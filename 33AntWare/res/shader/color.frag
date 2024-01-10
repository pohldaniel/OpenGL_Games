#version 410 core

uniform vec4 u_color = vec4(1.0f, 1.0f, 1.0f, 1.0f);

in vec3 normal;

out vec4 color;

void main(void){
	color = u_color;	
}
