#version 410 core

out vec4 outColor;

uniform vec4 u_color = vec4(1.0);

void main(void) {
	outColor = u_color;
}
