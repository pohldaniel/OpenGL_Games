#version 430 core

layout(location = 0) in vec4 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform vec4 u_color = vec4(1.0); 

out vec2 v_texCoord;
out vec3 v_normal;
out vec4 vertColor; 

void main(void){
   gl_Position = u_projection * u_view * u_model * i_position;
   v_texCoord = i_texCoord;
   v_normal = i_normal;
   vertColor = u_color;
}