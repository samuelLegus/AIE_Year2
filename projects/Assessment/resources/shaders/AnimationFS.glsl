#version 330

in vec2 TexCoord;

out vec4 outColor;

uniform sampler2D DiffuseTexture;

void main()
{
	outColor = texture2D(DiffuseTexture, TexCoord);
}