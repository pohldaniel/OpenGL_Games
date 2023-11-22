#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform vec3 u_eyePos;
uniform mat4 u_model;
uniform mat4 u_view;
uniform mat4 u_projection;

out vec4 P; // position
out vec3 N; // normal
out vec3 I; // incident vector

void main() {

	vec4 p1 = u_model * vec4(i_position, 1.0);
	vec3 n1 = mat3(u_model) * i_normal;
    N = normalize(n1);
	vec3 i1 = p1.xyz - u_eyePos;

	P = p1;
    N = n1;
    I = i1;

	gl_Position = u_projection * u_view * p1;
	
}