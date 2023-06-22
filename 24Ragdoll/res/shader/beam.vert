#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoord;
layout (location = 2) in vec3 i_normal;
layout (location = 5) in mat4 i_model;
layout (location = 9) in vec4 i_color;
layout (location = 10) in mat4 i_modelDyn;
layout (location = 14) in float i_maxHeight;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_normal;

out vec3 pos_model;
out vec3 pos_eye;
out vec3 normal_model;
out vec3 normal_eye;
out vec2 texCoord;
out vec4 vertColor;
flat out float maxHeight;

void main(void){
 
	pos_model    = i_position;
	pos_eye      = vec3(u_view * i_model * i_modelDyn * vec4(i_position, 1.0));
    normal_model = normalize(i_normal);
	//not neccesary for cylinder
	//normal_eye 	 = normalize(mat3(u_normal * inverse(i_modelDyn)) * i_normal);
	normal_eye 	 = normalize(mat3(u_normal) * i_normal);
	texCoord 	 = i_texCoord;
	vertColor 	 = i_color;
	maxHeight	 = i_maxHeight;
	gl_Position  = u_projection * u_view * i_model * i_modelDyn * vec4(i_position, 1.0);
}

//((u_modelView * i_model * i_modelDyn)^T)^I = ((i_model* i_modelDyn)^T * u_modelView^T)^I = u_modelView^T^I * (i_model * i_modelDyn)^T^I = u_normal * (i_model * i_modelDyn)^T^I