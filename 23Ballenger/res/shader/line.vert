#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec4 i_color;
layout (location = 10) in mat4 i_modelDyn;

uniform mat4 u_projection;
uniform mat4 u_view;

out vec4 vertColor;

void main(void){
	gl_Position  = u_projection * u_view * i_modelDyn * vec4(i_position, 1.0);
	vertColor = i_color;
}
