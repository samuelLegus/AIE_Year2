#version 400

in vec4 fColour;
in vec2 texCoord;

uniform sampler2D textureMap;

void main()
{
	gl_FragColor = texture( textureMap, texCoord ) * fColour;
}