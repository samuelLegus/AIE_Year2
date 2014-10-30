#version 400

layout(location = 0) in vec4 position;
layout(location = 1) in vec4 colour; // we'll ignore it
layout(location = 2) in vec2 textureCoordinate;

out vec2 texCoord;
out vec4 fColour;

uniform mat4 view;
uniform mat4 projection;

void main()
{
	texCoord = textureCoordinate;
	fColour = colour;
	gl_Position = projection * view * position;
};