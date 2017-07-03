#version 300 es
precision highp float;
layout(location = 0) in vec4 vertexAttribute0;
out vec3 cubemapDirection;
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

mat4 rotationMatrix(vec3 axis, float angle)
{
    axis = normalize(axis);
    float s = sin(angle);
    float c = cos(angle);
    float oc = 1.0 - c;
    
    return mat4(oc * axis.x * axis.x + c,           oc * axis.x * axis.y - axis.z * s,  oc * axis.z * axis.x + axis.y * s,  0.0,
                oc * axis.x * axis.y + axis.z * s,  oc * axis.y * axis.y + c,           oc * axis.y * axis.z - axis.x * s,  0.0,
                oc * axis.z * axis.x - axis.y * s,  oc * axis.y * axis.z + axis.x * s,  oc * axis.z * axis.z + c,           0.0,
                0.0,                                0.0,                                0.0,                                1.0);
}
void main()
{
	// every point of SkyBox is related to a point on Cubemap
	cubemapDirection = vertexAttribute0.xyz;
	// rotate and project SkyBox to camera
	vec4 position = worldToView * vec4(vertexAttribute0.xyz, 0.0);
	position.w = 1.0;
	gl_Position = (viewToProjection * position).xyww;
}
