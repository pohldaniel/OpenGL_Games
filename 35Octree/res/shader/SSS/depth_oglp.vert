#version 430 core

layout(location = 0) in vec3 i_position;

uniform mat4 u_lightViewProj = mat4(1.0);
uniform mat4 u_model = mat4(1.0);

void main() {
  gl_Position = u_lightViewProj * u_model * vec4(i_position, 1.0);
}
