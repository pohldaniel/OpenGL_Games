#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;

out vec3 v_Normal;
out vec4 v_fragPos;

void main(void) {
    
	v_fragPos = u_modelView * vec4(i_position, 1.0);
    v_Normal = mat3(u_normal) * i_normal;
	
	gl_Position = u_projection * v_fragPos;
}