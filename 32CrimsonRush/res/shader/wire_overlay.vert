#version 410 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoord;
layout (location = 2) in vec3 i_normal;

uniform mat4 u_mvp;
uniform mat4 u_model;

out vec2 texCoord;
out vec3 normal;
out vec3 position;
out vec3 worldPos;

void main(){

    vec4 pos = vec4(i_position, 1.0);
    gl_Position = u_mvp * pos;
    texCoord = i_texCoord;
    normal = i_normal;
    position = i_position;
    worldPos = (u_model * pos).xyz;
}
