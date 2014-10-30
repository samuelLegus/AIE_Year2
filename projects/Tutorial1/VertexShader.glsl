#version 150

in vec4 Position;
in vec4 Color;
out vec4 vColor;
uniform mat4 ProjectionView;

void main()
{
	vColor = Color;
	gl_Position = ProjectionView;
}