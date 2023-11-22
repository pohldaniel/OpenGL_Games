#version 410 core

in vec3 v_texCoord;

out vec4 color;

uniform samplerCube u_texture;

void main(void){
	color = texture(u_texture, v_texCoord);
}