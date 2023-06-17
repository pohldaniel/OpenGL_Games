#version 410 core

in vec2 texCoord;

out vec4 color;

uniform sampler2D u_texture;

void main(){
	color = texture2D( u_texture, texCoord);	
}