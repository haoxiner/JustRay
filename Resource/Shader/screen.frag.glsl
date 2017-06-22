#version 300 es
precision highp float;
in vec2 texCoord;

void main()
{
	gl_Position = vec4(positionxy, 0.0, 1.0);
}
