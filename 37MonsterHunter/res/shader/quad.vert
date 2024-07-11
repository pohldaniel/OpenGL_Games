#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

uniform mat4 u_transform = mat4(1.0);
uniform vec4 u_color = vec4(1.0);
uniform vec4 u_texRect = vec4(0.0, 0.0, 1.0, 1.0);
uniform bool u_flip = false;

out vec2 v_texCoord;
out vec4 v_color; 

void main(void){
  gl_Position = u_transform * vec4(i_position, 1.0);
  
  v_texCoord.y =  u_flip ? 1.0 - i_texCoord.y : i_texCoord.y;
  v_texCoord.x = i_texCoord.x * (u_texRect.z - u_texRect.x) + u_texRect.x;  
  v_texCoord.y = v_texCoord.y * (u_texRect.w - u_texRect.y) + u_texRect.y;

  v_color = u_color;
}