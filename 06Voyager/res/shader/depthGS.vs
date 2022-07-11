#version 410 core

const int NUM_CASCADES = 4;

layout(location = 0) in vec3 i_position;


uniform mat4 u_viewShadows[NUM_CASCADES];
uniform mat4 u_projectionShadows[NUM_CASCADES];

uniform mat4 u_projection;
uniform mat4 u_modelView = mat4(1.0);

void main(void){

	//gl_Position = u_projectionShadows[0] * u_viewShadows[0] * vec4(i_position, 1.0);
	gl_Position = vec4(i_position, 1.0);
	
	//gl_Position = u_projection * u_modelView * vec4(i_position, 1.0);
}