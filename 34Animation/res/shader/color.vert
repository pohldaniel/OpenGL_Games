#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_normal;

out vec3 normal;

void main(void){
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0); 
	normal = normalize(mat3(u_normal) * i_normal);
}