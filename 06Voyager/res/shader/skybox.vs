#version 410 core
layout(location = 0) in vec3 i_position;

out vec3 v_texCoord;

uniform mat4 u_transform;

void main(void){
   gl_Position =  u_transform * vec4(i_position, 1.0);
   v_texCoord = i_position;
  
}