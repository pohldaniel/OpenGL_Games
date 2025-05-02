#version 410 core

layout (location = 0) in vec3 inPos;
layout (location = 2) in vec3 inNormal;

out vec3 vertNormal;
out vec3 viewDir;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);

void main() {
	vertNormal = (u_normal * vec4(inNormal, 0.0)).xyz;
	vec4 viewSpace = u_view * vec4(inPos, 0);
	viewDir = vec3(viewSpace);
	gl_Position = u_projection * u_view * u_model * vec4(inPos, 1);
}