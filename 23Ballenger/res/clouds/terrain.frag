#version 410 core

in vec3 normal;
in vec3 lightDirection;
in vec4 vertColor;

out vec4 color;

void main() {
	color = vertColor;
	float NdotLD = dot(normalize(normal), normalize(lightDirection));
	color.rgb *= 0.5 + 0.5 * NdotLD;
	
	//color = vec4(normal, 1.0);
}
