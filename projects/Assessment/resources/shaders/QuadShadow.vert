#version 410 core

layout(location = 0) in vec4 Position;	
layout(location = 1) in vec2 TexCoord;

out vec2 UV;
	
void main()
{
	UV = TexCoord;
	gl_Position = Position;
}