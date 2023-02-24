#version 410 core

layout(location = 0) in vec4 i_position;

out vec4 position;

void main(void) {

	position = i_position;
}