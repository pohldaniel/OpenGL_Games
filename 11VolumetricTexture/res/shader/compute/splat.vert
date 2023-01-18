#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoords;

out vec3 v_texCoord;

void main(void) {
	gl_Position = vec4(position, 1.0);
	v_texCoord = texCoords;
}