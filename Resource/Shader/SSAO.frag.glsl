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

const int kernelSize = 32;
const float bias = 1e-3;
const float radius = 0.5;

void main()
{
    occlusion = 0.0;
    
    float depth = texture(depthBuffer, texCoord).r * 2.0 - 1.0;
    
    if (depth > 0.999) {
        return;
    }
    vec2 noiseScale = vec2(settings.xy);
    
    vec4 projectedPosition = vec4(texCoord*2.0 - vec2(1.0), depth, 1.0);
    vec4 positionInCameraSpace = inverse(viewToProjection) * projectedPosition;
    positionInCameraSpace /= positionInCameraSpace.w;
    
    vec3 normal = normalize(mat3(worldToView) * (texture(normalMap, texCoord).xyz * 2.0 - vec3(1.0)));
    vec3 randomVec = texture(noiseMap, texCoord * noiseScale).xyz;
    vec3 tangent   = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN       = mat3(tangent, bitangent, normal);
    for(int i = 0; i < kernelSize; i++) {
        // get sample position
        vec4 positionSample = vec4(positionInCameraSpace.xyz + TBN * kernel[i].xyz * radius, 1.0);
        positionSample = viewToProjection * positionSample;
        positionSample /= positionSample.w;
        positionSample.xyz = positionSample.xyz * 0.5 + vec3(0.5);
        float depthSample = texture(depthBuffer, positionSample.xy).r;
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(depth - depthSample));
        occlusion += (depthSample >= positionSample.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = (occlusion / float(kernelSize));
}
