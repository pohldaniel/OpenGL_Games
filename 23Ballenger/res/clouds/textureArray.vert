#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

uniform vec4 u_color = vec4(1.0, 0.0, 1.0, 1.0);

out vec4 vertColor;

void main(void){
	gl_Position = vec4(i_position, 1.0);  
	vertColor = u_color;
}