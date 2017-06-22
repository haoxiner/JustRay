#version 300 es
precision highp float;
precision highp sampler2D;

in vec3 position;
in vec3 tangent;
in vec3 bitangent;
in vec3 normal;
in vec2 texCoord;

layout (location = 0) out vec4 gBuffer0;
layout (location = 1) out vec3 gBuffer1;
layout (location = 2) out vec2 gBuffer2;

layout(std140) uniform PerEngineBuffer
{
	mat4 viewToProjection;
};
layout(std140) uniform PerFrameBuffer
{
	vec4 cameraPosition;
	mat4 worldToView;
};
layout(std140) uniform PerObjectBuffer
{
	mat4 modelToWorld;
	vec4 material[2];
};

uniform sampler2D baseColorMap;
uniform sampler2D roughnessMap;

vec2 EncodeNormal(vec3 n)
{
    float p = sqrt(n.z*8.0+8.0);
    return vec2(n.xy/p + vec2(0.5)) * 0.5 + vec2(0.5);
}

void main()
{
    vec3 N = normalize(normal);
    
    vec3 baseColor = vec3(1.0);
	float roughness = 1.0;
	float metallic = 1.0;
    
    if (material[0].x > 0.0) {
        vec2 uv = texCoord * material[0].y;
        vec4 baseColorAndMetallic = texture(baseColorMap, uv);
        vec4 normalAndRoughness = texture(roughnessMap, uv);
        
        baseColor = baseColorAndMetallic.rgb;
        metallic = baseColorAndMetallic.a;
        roughness = normalAndRoughness.a;
        
        vec3 T = normalize(tangent);
        vec3 B = normalize(bitangent);
        mat3 TBN = mat3(T,B,N);
        N = TBN * normalize(normalAndRoughness.xyz * 2.0 - vec3(1.0));
    }
    metallic *= material[0].z;
    baseColor *= material[1].xyz;
    roughness *= material[1].w;
    
//    baseColor = vec3(1.0);
//    roughness = 0.2;
//    metallic = 1.0;
//    N = normalize(normal);
    
    
    gBuffer0 = vec4(baseColor, metallic);
//    gBuffer1 = vec4(EncodeNormal(N),roughness, 0.0);
    gBuffer1 = N;
    gBuffer2 = vec2(roughness);
}
