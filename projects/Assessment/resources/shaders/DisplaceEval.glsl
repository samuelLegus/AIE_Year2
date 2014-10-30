#version 420

layout(triangles, equal_spacing, ccw) in;

in vec2 texCoord[];
out vec2 TexCoord;

uniform mat4 viewProjection;

uniform sampler2D displacementMap;

uniform float displacementScale;

void main()
{
	// TexCoords
	vec2 t0 = gl_TessCoord.x * texCoord[0];
	vec2 t1 = gl_TessCoord.y * texCoord[1];
	vec2 t2 = gl_TessCoord.z * texCoord[2];

	TexCoord = t0 + t1 + t2;

	float offset = texture(displacementMap, TexCoord ).r * displacementScale;

	// Positions
	vec3 p0 = gl_TessCoord.x * gl_in[0].gl_Position.xyz;
	vec3 p1 = gl_TessCoord.y * gl_in[1].gl_Position.xyz;
	vec3 p2 = gl_TessCoord.z * gl_in[2].gl_Position.xyz;

	vec3 position = p0 + p1 + p2;

	position.y += offset;

	gl_Position = viewProjection * vec4(position, 1);
}