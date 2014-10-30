#version 440

layout(location = 0) in vec4 Position;
layout(location = 1) in vec4 Normal;

uniform mat4 view;
uniform mat4 projection;

out vec3 position;
out vec3 normal;

void main()
{
	position = Position.xyz;
	normal = Normal.xyz;
	gl_Position = projection * view * Position;
}