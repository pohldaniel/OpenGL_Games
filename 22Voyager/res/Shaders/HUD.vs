#version 440 core
layout (location = 0) in vec3 vertex_position;
layout (location = 1) in vec2 vertex_Uv;

out vec2 VertexUv; 

uniform mat4 projection;
uniform mat4 model;

void main(){
    gl_Position = projection * model * vec4(vertex_position, 1.0f);
	VertexUv = vertex_Uv;
}