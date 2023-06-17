#version 410 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoords;

out vec2 texCoord;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model = mat4(1.0);

void main(){
	gl_Position = projection * view * model * vec4(i_position, 1.0f);
	texCoord = i_texCoords;
}