#version 410 core

layout(location = 0) in vec4 i_position;

uniform mat4 u_projection;
uniform mat4 u_modelView;

out vec4 v_gFragPos;

void main(){
	v_gFragPos = u_modelView * i_position;
	gl_Position = u_projection * v_gFragPos;
}