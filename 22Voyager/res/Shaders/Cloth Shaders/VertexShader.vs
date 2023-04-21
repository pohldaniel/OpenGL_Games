#version 440 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 normal;

out vec2 vUV;
out vec3 vNormal;

uniform mat4 mvp;
uniform mat4 view;

void main () {
	mat3 normalMatrix = inverse(transpose(mat3(view)));
    vNormal = normalize(normalMatrix * normal);
    vUV = uv;
    gl_Position = mvp * vec4(position, 1.0f);
}