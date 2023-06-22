#version 410 core

uniform sampler3D u_texture;

in vec3 texCoord;

out vec4 color;

void main(void){
	color = texture( u_texture, texCoord );	
}
