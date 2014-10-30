#version 150

in vec2 TexCoord;
in vec4 Color;

uniform sampler2D textureMap;

void main()
{
	gl_FragColor = texture(textureMap, TexCoord);
}