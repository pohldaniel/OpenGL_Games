#version 430 core

layout(location = 0) in vec3 position;

uniform mat4 u_modelviewProjection;

out vec4 vPosition;

void main() {
    gl_Position = u_modelviewProjection * vec4(position, 1.0);
    vPosition = vec4(position, 1.0);
}