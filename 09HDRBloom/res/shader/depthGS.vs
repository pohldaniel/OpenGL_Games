#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 texCoord;

uniform mat4 u_model = mat4(1.0);

void main(void){
	gl_Position = u_model * vec4(i_position, 1.0);	
	texCoord = i_texCoord;
}