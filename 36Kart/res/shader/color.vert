#version 410 core

layout(location = 0) in vec3 i_position;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform vec4 u_color = vec4(1.0); 

out vec4 vertColor; 

void main(void){
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0); 
	vertColor = u_color;
}