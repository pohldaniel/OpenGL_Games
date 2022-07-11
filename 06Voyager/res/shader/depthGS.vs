#version 410 core

layout(location = 0) in vec3 i_position;

void main(void){
	gl_Position = vec4(i_position, 1.0);	
}