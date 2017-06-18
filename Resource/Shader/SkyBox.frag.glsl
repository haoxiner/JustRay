#version 300 es
precision highp float;
precision highp samplerCube;

in vec3 cubemapDirection;
out vec4 fragColor;
layout(binding = 0) uniform samplerCube envmap;
layout(binding = 1, std140) uniform PerFrameBuffer
{
	vec4 cameraPosition;
	mat4 worldToView;
};
vec4 SampleCubemapForZup(samplerCube cubemap, vec3 direction)
{
	vec4 color = texture(cubemap, vec3(direction.x, direction.z, -direction.y));
	return color;
}
void main()
{
	fragColor = SampleCubemapForZup(envmap, cubemapDirection);
	fragColor.w = 1.0;
}
