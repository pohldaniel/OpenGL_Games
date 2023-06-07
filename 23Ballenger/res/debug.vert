#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

uniform uint u_layer;

out vec2 texCoord;
flat out uint layer;

void main(void){
	gl_Position = vec4(i_position, 1.0);
	texCoord = i_texCoord;  
	layer = u_layer;
}