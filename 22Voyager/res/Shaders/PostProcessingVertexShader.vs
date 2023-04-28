#version 440 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_uv;

out vec2 VertexUv; 

void main() {
    gl_Position = vec4(vertex_position, 1.0f);
	VertexUv = vertex_uv;
}