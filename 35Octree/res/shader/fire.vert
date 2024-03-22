#version 430 core
layout(location = 0) in vec4 i_position;

uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);

void main(){
	gl_Position = u_view * u_model * i_position;
}