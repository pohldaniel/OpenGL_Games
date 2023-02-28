#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texccord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;

out vec2 v_texccord;
out vec3 v_normal;
out vec4 v_fragPos;
 
void main(void) {

	v_fragPos = u_modelView * vec4(i_position, 1.0);
	
	gl_Position = u_projection * v_fragPos;	
	v_normal = mat3(u_normal) * i_normal;
	v_texccord = i_texccord;
}