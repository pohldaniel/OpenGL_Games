#version 410 core

in vec2 texCoord;
in vec3 normal;
in vec3 position;

out vec4 color;

uniform samplerCube u_cubeMap;

void main() {

	vec3 R = reflect(normalize(position), normalize(normal));
	color =	texture(u_cubeMap, R);
}