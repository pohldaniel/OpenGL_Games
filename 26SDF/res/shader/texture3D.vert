#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_texCoord;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 texCoord;

void main(){
	texCoord = i_texCoord;
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
}
