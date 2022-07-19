#version 410 core

layout(location = 0) in vec3 i_position;

out vec4 v_color;

uniform vec4 u_color = vec4(1.0);
uniform mat4 u_transform;

void main(void){
   gl_Position = u_transform * vec4(i_position, 1.0);
   v_color = u_color;
}