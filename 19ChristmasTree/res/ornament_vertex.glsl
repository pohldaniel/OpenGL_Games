#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;

uniform mat4 u_projection;
uniform mat4 u_modelView;

out vec3 position;
out vec4 mat;

void main() {
	
	vec4 fragPos = u_modelView * vec4(i_position, 1.0);
	gl_Position = u_projection * fragPos;
	position = fragPos.xyz;
	
	mat = i_color;
}