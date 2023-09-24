#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 texCoord;
out vec4 vertColor;

uniform vec4 u_texRect = vec4(0.0, 0.0, 1.0, 1.0);
uniform bool u_flip = true;

void main(void){
	gl_Position = vec4(i_position, 1.0);

	texCoord.y =  u_flip ? 1.0 - i_texCoord.y : i_texCoord.y;

	texCoord.x = i_texCoord.x * (u_texRect.z - u_texRect.x) + u_texRect.x;  
	texCoord.y = texCoord.y * (u_texRect.w - u_texRect.y) + u_texRect.y;
	vertColor = vec4(1.0);
}