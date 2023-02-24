#version 410 core

layout(location = 0) in vec4 i_position;

uniform mat4 u_projection;
uniform mat4 u_modelView;


void main(){
	gl_Position = u_projection * u_modelView * i_position;
}