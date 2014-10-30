#version 330

layout( location = 0) in vec4 position;
layout( location = 1) in vec4 color;
layout( location = 2) in vec2 texCoord;

out vec2 TexCoord;
out vec4 Color;

uniform mat4 viewProjection;
uniform mat4 global;

void main()
{
	TexCoord = texCoord;
	Color = color;
	gl_Position = position;
}