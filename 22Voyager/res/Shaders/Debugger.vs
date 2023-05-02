#version 440 core
layout (location = 0) in vec3 vertex_position;

out vec4 vertexColor; 

uniform mat4 view;
uniform mat4 projection;

void main() {
    gl_Position = projection * view * vec4(vertex_position, 1.0f);
}