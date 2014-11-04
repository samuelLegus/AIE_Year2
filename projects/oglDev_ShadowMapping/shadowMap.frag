#version 330

in vec2 TexCoord2Out;
uniform sampler2D gShadowMap;

out vec4 FragColor;

void main()
{
	//This is voodoo magic math that allows for greater z precision.
	//See http://ogldev.atspace.co.uk/www/tutorial23/tutorial23.html in the fragment shader section.
	float depth = texture(gShadowMap, TexCoordOut).x
	depth = 1.0 - (1.0 - depth) * 25.0;
	FragColor = vec4(depth);
}