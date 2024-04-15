#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

out VS_GS_VERTEX{
	vec3 normal;
	vec2 texCoord;
} vertex_out;

void main(void){
	vertex_out.normal = i_normal;
	vertex_out.texCoord = i_texCoord;
	gl_Position = vec4(i_position, 1.0);
}