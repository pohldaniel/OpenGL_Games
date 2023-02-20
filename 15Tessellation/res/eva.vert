#version 120

in vec3 i_position;
in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;

varying vec3 v_Normal;
varying vec4 v_fragPos;

void main(void) {
    
	v_fragPos = u_modelView * vec4(i_position, 1.0);
    v_Normal = mat3(u_normal) * gl_Normal;
	
	gl_Position = u_projection * v_fragPos;
}