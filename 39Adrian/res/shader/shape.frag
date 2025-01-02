#version 410 core

uniform sampler2D u_texture;

in vec2 texCoord;
in vec3 normal;
in vec4 vertColor;

out vec4 color;

void main(void){
	color = vertColor * texture2D( u_texture, texCoord );	
}
