#version 410 core

uniform sampler2D u_texture;

in vec2 vertTexCoord;

out vec4 colorOut;

void main(void){
	colorOut = texture2D( u_texture, vertTexCoord );
}