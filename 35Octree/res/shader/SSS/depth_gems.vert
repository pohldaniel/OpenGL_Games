#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_lightProj = mat4(1.0);
uniform mat4 u_lightView = mat4(1.0);
uniform mat4 u_model = mat4(1.0);

out float distance;

void main() {
  gl_Position = u_lightProj * u_lightView * u_model * vec4(i_position + i_normal * 0.003, 1.0);
  distance = length(u_lightView * u_model * vec4(i_position, 1.0));
}
