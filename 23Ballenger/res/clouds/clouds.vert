#version 430

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
           
uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec2 tex_coord;

void main(void){
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
   
	tex_coord = i_texCoord;  
}