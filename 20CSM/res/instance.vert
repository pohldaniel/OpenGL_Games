#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in mat4 i_model;

uniform mat4 u_projection;
uniform mat4 u_modelView;

uniform mat4 u_view;

uniform mat4 u_normal;
uniform mat4 u_model;

out vec3 v_normal;
out vec4 v_fragPos;

void main(void) {

	v_fragPos = u_view * i_model * vec4(i_position, 1.0);

	gl_Position = u_projection * v_fragPos;
	
	v_normal = mat3(u_normal * i_model)  * i_normal;
}

//((u_modelView * i_model)^T)^I = (i_model^T * u_modelView^T)^I = u_modelView^T^I * i_model^T^I = u_normal * i_model