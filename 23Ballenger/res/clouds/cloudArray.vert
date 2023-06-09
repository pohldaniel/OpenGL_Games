#version 410 core
#extension GL_EXT_gpu_shader4 : enable

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

uniform vec4 u_color = vec4(1.0, 0.0, 1.0, 1.0);

out vec4 vertColor;
out vec2 vUvs;

void main(void){
	gl_Position = vec4(i_position, 1.0);  
	vUvs = i_texCoord;
	vertColor = u_color;
}