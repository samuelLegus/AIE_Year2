#version 420

// shove 3 verts into the patch
// shove'em out unmodded

layout(vertices = 3) out;

in vec2 TexCoord[];
out vec2 texCoord[];

uniform float displacementLevel;

void main()
{
	gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
	texCoord[gl_InvocationID] = TexCoord[gl_InvocationID];

	if (gl_InvocationID == 0)
	{
		gl_TessLevelInner[0] = displacementLevel;

		gl_TessLevelOuter[0] = displacementLevel;
		gl_TessLevelOuter[1] = displacementLevel;
		gl_TessLevelOuter[2] = displacementLevel;
	}
}