#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoord;
layout (location = 2) in vec3 i_normal;
layout (location = 5) in mat4 i_model;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_normal;

layout (std140, binding = 0) uniform u_color{
	vec4 color[7];
};

out vec4 vertColor;

void main(void){
	gl_Position = u_projection * u_view * i_model * vec4(i_position, 1.0);
	vertColor = color[gl_InstanceID];
}