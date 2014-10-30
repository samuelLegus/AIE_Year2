#version 440

in vec3 normal;
in vec3 position;

uniform vec3 lightAmbient;
uniform vec3 lightDirection;
uniform vec3 lightColour;
uniform vec3 lightSpecular;
uniform vec3 cameraPosition;

void main()
{
	vec3 N = normalize(normal);
	vec3 L = normalize(-lightDirection);
	vec3 R = reflect(-L, N);
	vec3 E = normalize( cameraPosition - position);

	float d = max(0, dot(N, L));

	vec3 diffuse = lightColour * d;
	vec3 ambient = lightAmbient;

	float s = pow( max(0, dot(E,R) ), 128);

	vec3 specular = lightSpecular * s;

	gl_FragColor = vec4( lightAmbient + diffuse + specular , 1);
}