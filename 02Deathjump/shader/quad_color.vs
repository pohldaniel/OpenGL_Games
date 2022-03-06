#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;

out vec4 color;

uniform mat4 u_transform = mat4(1.0);

void main(void) {
    gl_Position = u_transform * vec4(i_position, 1.0);
    color  = i_color;
}
