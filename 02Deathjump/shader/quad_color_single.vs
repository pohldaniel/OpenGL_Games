#version 330

layout(location = 0) in vec3 i_position;

out vec4 color;

uniform vec4 u_color;
uniform mat4 u_transform = mat4(1.0);

void main(void) {
    gl_Position = u_transform * vec4(i_position, 1.0);
    color  = u_color;
}
