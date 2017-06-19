#version 300 es
precision highp float;
precision highp int;
precision highp sampler2D;
precision highp samplerCube;

#define PI 3.1415926535897932384626433832795
#define INV_PI (1.0/PI)
#define TWO_PI (2.0 * PI)

in vec3 position;
in vec3 tangent;
in vec3 bitangent;
in vec3 normal;
in vec2 texCoord;
//layout(std140) uniform PerEngineBuffer
//{
//	mat4 viewToProjection;
//};
//layout(std140) uniform PerFrameBuffer
//{
//	vec4 cameraPosition;
//	mat4 worldToView;
//};
//layout(std140) uniform PerObjectBuffer
//{
//	mat4 modelToWorld;
//	vec4 material[2];
//};
uniform sampler2D baseColorMap;
uniform sampler2D roughnessMap;

vec3 ApproximationSRgbToLinear(in vec3 sRGBCol)
{
	return pow(sRGBCol , vec3(2.2));
}

vec3 ApproximationLinearToSRGB(in vec3 linearCol)
{
	return pow(linearCol , vec3(1.0 / 2.2));
}

out vec4 fragColor;


void main()
{
	fragColor = texture(baseColorMap, texCoord);
    fragColor.xyz = ApproximationLinearToSRGB(fragColor.xyz);
    fragColor.w = 1.0;
}
