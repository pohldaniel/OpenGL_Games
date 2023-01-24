#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_transform;

out vec3 v_normal;

void main(void){
   gl_Position = u_transform * vec4(i_position, 1.0);
   v_normal = i_normal;
}