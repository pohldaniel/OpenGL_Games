#version 410 core

in vec3 v_normal;
in vec2 v_texCoord;

out vec4 color;

void main(void){
	color = vec4 (v_normal, 1.0);
}
