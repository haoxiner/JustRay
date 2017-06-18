#version 300 es
precision highp float;
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexNormal;
layout(location = 2) in vec4 vertexTangent;
layout(location = 3) in vec4 vertexBitangent;
layout(location = 4) in vec2 vertexTexCoord;
out vec3 position;
out vec3 tangent;
out vec3 bitangent;
out vec3 normal;
out vec2 texCoord;
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
void main()
{
	position = (modelToWorld * vec4(vertexPosition, 1.0)).xyz;
	normal = normalize(modelToWorld * vec4(vertexNormal.xyz, 0.0)).xyz;
    tangent = normalize(modelToWorld * vec4(vertexTangent.xyz, 0.0)).xyz;
    bitangent = normalize(modelToWorld * vec4(vertexBitangent.xyz, 0.0)).xyz;
	texCoord = vertexTexCoord;

	gl_Position = viewToProjection * worldToView * vec4(position, 1.0);
//    gl_Position = vec4(vertexPosition, 1.0);
}
