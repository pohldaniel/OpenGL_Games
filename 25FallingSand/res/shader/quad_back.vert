#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

uniform mat4 u_transform = mat4(1.0);
uniform vec4 u_color = vec4(1.0);
uniform vec4 u_texRect = vec4(0.0, 0.0, 1.0, 1.0);

out vec2 texCoord;
out vec4 vertColor;

void main(void){
	gl_Position = u_transform * vec4(i_position, 1.0);
	gl_Position.z = gl_Position.w;
	
	texCoord.x = i_texCoord.x * (u_texRect.z - u_texRect.x) + u_texRect.x;  
	texCoord.y = i_texCoord.y * (u_texRect.w - u_texRect.y) + u_texRect.y;
	vertColor = u_color;
}