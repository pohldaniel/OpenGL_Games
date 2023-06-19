// simple vertex shader
#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_normal;

out vec3 pos_model;
out vec3 pos_eye;
out vec3 normal_model;
out vec3 normal_eye;
out vec2 texCoord;

void main(){

	texCoord 	 = i_texCoord; 
    pos_model    = i_position;
    pos_eye      = vec3(u_view * u_model * vec4(i_position, 1.0));
    normal_model = normalize(i_normal);
    normal_eye   = normalize(mat3(u_normal) * i_normal);

    gl_Position	 = u_projection * u_view * u_model * vec4(i_position, 1.0); 
}
