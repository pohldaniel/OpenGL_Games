#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_normal;

out vec2 texCoord;
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;

void main(void){
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
   
	texCoord = i_texCoord;  
	normal = normalize(mat3(u_normal) * i_normal);
}