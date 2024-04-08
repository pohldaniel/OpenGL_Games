#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;


uniform mat4 u_viewProjection;
uniform mat4 u_model = mat4(1.0);

void main() {
    gl_Position = u_viewProjection * u_model * vec4(i_position, 1.0f);
}
