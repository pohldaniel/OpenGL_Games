#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec4 i_color;

out vec4 v_color;

uniform mat4 u_transform;

void main(void){
   gl_Position = u_transform * vec4(i_position, 1.0);
   v_color = i_color;
}