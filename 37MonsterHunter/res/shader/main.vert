#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform vec4 u_color = vec4(1.0); 

out vec2 v_texCoord;
out vec4 v_color; 

void main(void){
   gl_Position = u_projection * u_view * vec4(i_position, 1.0);
  
   v_texCoord = i_texCoord;
   v_color = u_color;
}
