#version 300 es
precision highp float;
precision highp sampler2D;
in vec2 texCoord;
out float occlusion;
layout(std140) uniform PerEngineBuffer
{
	mat4 viewToProjection;
};
layout(std140) uniform PerFrameBuffer
{
	vec4 cameraPosition;
	mat4 worldToView;
};
layout(std140) uniform SSAOBuffer
{
    vec4 settings;
    vec4 kernel[64];
};
uniform sampler2D normalMap;
uniform sampler2D depthBuffer;
uniform sampler2D noiseMap;

const int kernelSize = 8;
const float bias = 0.005f;
const float radius = 0.45f;

void main()
{
    occlusion = 0.0;
    float depth = texture(depthBuffer, texCoord).r;
    if (depth > 0.999f) {
        occlusion = 1.0;
        return;
    }
    vec4 origin = inverse(viewToProjection) * vec4(texCoord * 2.0 - vec2(1.0), depth * 2.0 - 1.0, 1.0);
    origin /= origin.w;
    
    vec4 normal = vec4(texture(normalMap, texCoord).xyz * 2.0 - vec3(1.0), 0.0);
    normal = normalize(worldToView * normal);
    
    vec3 rvec = texture(noiseMap, texCoord * settings.xy).xyz * 2.0 - vec3(1.0);
    vec3 tangent = normalize(rvec - normal.xyz * dot(rvec, normal.xyz));
    vec3 bitangent = cross(normal.xyz, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal.xyz);
    
    for (int i = 0; i < kernelSize; i++) {
        vec3 posSample = TBN * kernel[i].xyz * radius + origin.xyz;
        vec4 offset = vec4(posSample, 1.0);
        offset = viewToProjection * offset;
        offset /= offset.w;
        
        float depthSample = texture(depthBuffer, offset.xy * 0.5 + vec2(0.5)).r;
        offset.z = depthSample * 2.0 - 1.0;
        offset = inverse(viewToProjection) * offset;
        offset /= offset.w;
        
        float rangeCheck = abs(origin.z - offset.z) < radius ? 1.0 : 0.0;
        occlusion += (offset.z >= posSample.z ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0f - (occlusion / float(kernelSize));
}
