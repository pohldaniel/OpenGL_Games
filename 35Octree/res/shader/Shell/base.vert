#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_model = mat4(1.0);

out VS_FS_VERTEX{
	vec3 normal;
} vertex_out;

void main(void){
	vertex_out.normal = i_normal;
	gl_Position = u_projection * (u_model * vec4(i_position, 1.0));
}