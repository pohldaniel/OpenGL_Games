#version 410 core

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec4 i_color;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model = mat4(1.0);

out vec4 v_color;

void main(void){
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 0.0, 1.0); 
	v_color = i_color;
	//gl_PointSize = 10.0;
}