#version 410 core
layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;

uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normal;

out vec2 texCoord;
out vec3 normal;

void main(){
    gl_Position = u_model * vec4(i_position, 1.0);
	texCoord = i_texCoords;
	normal = i_normal;
}