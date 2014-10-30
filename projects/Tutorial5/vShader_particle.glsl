#version 440

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Colour;

out vec4 colour;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	colour = Colour;
	gl_Position = projection * view * Position;
}