#version 440 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_uv;
layout (location = 2) in vec3 vertex_normals;

out vec2 vertexUv;
out vec3 vertexNorms;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {

    gl_Position = projection * model * vec4(vertex_position, 1.0f);
	vertexUv = vertex_uv;
	fragPos = vec3(model * vec4(vertex_position, 1.0f));
	vertexNorms = mat3(transpose(inverse(model))) * vertex_normals;
}