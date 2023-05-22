#version 440 core

uniform sampler2D u_texture;

in vec2 texCoord;
in vec3 normal;

out vec4 color;

void main(void){
	color = texture(u_texture, texCoord);		
}
