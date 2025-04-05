#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec4 i_color;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model = mat4(1.0);
uniform vec4 u_color = vec4(1.0);

out vec2 texCoord;
out vec3 normal;
out vec4 vertColor;

void main(void){
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0); 
	texCoord = i_texCoord;
	normal = i_normal;
	vertColor = i_color;
}