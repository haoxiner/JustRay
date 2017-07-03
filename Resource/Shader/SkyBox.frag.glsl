#version 300 es
precision highp float;
precision highp samplerCube;

in vec3 cubemapDirection;
out vec4 fragColor;
layout(std140) uniform PerEngineBuffer
{
	mat4 viewToProjection;
};
layout(std140) uniform PerFrameBuffer
{
    vec4 perFrameData;
	vec4 cameraPosition;
	mat4 worldToView;
};

uniform samplerCube specularEnvmap;

float A = 0.15;
float B = 0.50;
float C = 0.10;
float D = 0.20;
float E = 0.02;
float F = 0.30;
float W = 11.2;

vec3 Uncharted2Tonemap(vec3 x) {
   return ((x * (A * x + C * B) + D * E) / (x * (A * x + B) + D * F)) - E / F;
}

//Based on Filmic Tonemapping Operators http://filmicgames.com/archives/75
vec3 TonemapUncharted2(vec3 color) {
    float ExposureBias = 2.0;
    vec3 curr = Uncharted2Tonemap(ExposureBias * color);

    vec3 whiteScale = 1.0 / Uncharted2Tonemap(vec3(W));
    return curr * whiteScale;
}
vec3 ApproximationLinearToSRGB(in vec3 linearCol)
{
	return pow(linearCol , vec3(1.0 / 2.2));
}
vec4 SampleCubemapForZup(samplerCube cubemap, vec3 direction, float lod)
{
	vec4 color = textureLod(cubemap, vec3(direction.x, direction.z, -direction.y), lod);
	return color;
}
void main()
{
	fragColor = SampleCubemapForZup(specularEnvmap, cubemapDirection, 4.0);
    fragColor.xyz = TonemapUncharted2(fragColor.xyz);
    fragColor.xyz = ApproximationLinearToSRGB(fragColor.xyz);
	fragColor.w = 1.0;
//    fragColor.xyz = vec3(0.5,0.6,0.2);
}
