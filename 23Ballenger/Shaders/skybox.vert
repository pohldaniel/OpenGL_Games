#version 440 core
layout (location = 0) in vec3 i_position;

out vec3 texCoord;

uniform mat4 u_view;
uniform mat4 u_projection;

void main() {
    gl_Position = u_projection * u_view * vec4(i_position, 1.0f);
	//gl_Position.z = gl_Position.w;
	texCoord = i_position;
}