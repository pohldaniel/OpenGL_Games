#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 vTexCoord;

void main() {
	gl_Position = vec4(i_position, 1.0);
	vTexCoord = i_texCoord;
}