#version 410 core

layout(location = 0) in vec2 coord2d;

uniform mat4 transform;

void main(void) {
	gl_Position = transform * vec4(coord2d.xy, 0, 1);
}
