#version 410 core

layout(location = 0) in vec3 i_position;

out vec4 v_color;

uniform vec4 u_color = vec4(1.0);
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

void main(void){
   gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
   v_color = u_color;
}