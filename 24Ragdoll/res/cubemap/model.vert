#version 410 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;

out vec2 texCoord;
out vec3 normal;
out vec3 position;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normal;

void main(){
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0f);
	texCoord = i_texCoords;
	normal   = normalize(mat3(u_normal) * i_normal);
	position = vec3(u_view * u_model * vec4(i_position, 1.0f));
}