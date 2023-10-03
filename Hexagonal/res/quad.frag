#version 410 core

uniform sampler2D ImageTexture;

uniform float fWidth;
uniform float fHeight;

in vec2 texCoord;

out vec4 color;

void main(void){
	color = texture2D( ImageTexture, texCoord );
}