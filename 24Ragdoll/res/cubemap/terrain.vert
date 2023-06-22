// simple vertex shader
#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

out vec3 v_pos_model;
out vec4 v_pos_eye;
out vec3 v_normal_model;
out vec2 v_texCoord;

void main(){

	v_texCoord     = i_texCoord; 
    v_pos_model    = i_position;
    v_pos_eye      = u_model * vec4(i_position, 1.0); 
    v_normal_model = normalize(i_normal);
   

    gl_Position	 = v_pos_eye; 
}
