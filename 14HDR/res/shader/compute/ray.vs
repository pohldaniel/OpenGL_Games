#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec2 texCoords;

out vec3 vPosition;

void main() {
    gl_Position = vec4(position, 1.0);
    vPosition = position;
}
