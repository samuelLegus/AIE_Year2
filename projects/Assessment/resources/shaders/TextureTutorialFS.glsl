#version 330

in vec2 texCoord;

uniform sampler2D textureMap;

void main()
{
	gl_FragColor = texture( textureMap, texCoord );
}