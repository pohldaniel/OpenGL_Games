#version 410 core

in vec2 v_texCoord;
in vec3 v_normal;
in vec4 v_color;

out vec4 color;

void main(){
	color = v_color;
}