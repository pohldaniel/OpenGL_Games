#version 410 core

precision highp float;

uniform vec4 color;

out vec4 outColor;

void main(void) {
    outColor = color;
}
