#version 410 core

uniform sampler2D u_texture;

in vec2 texCoord;
in vec4 vertColor;

out vec4 color;

void main(void){
	color = texture2D( u_texture, texCoord ) * vertColor;	
}