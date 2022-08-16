#version 410 core

in vec3 v_color;
in vec2 v_texCoord;
in vec3 v_normal;

out vec4 color;

void main(void){
	color = vec4 (v_normal, 1.0);
}
