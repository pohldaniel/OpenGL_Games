#version 410 core

uniform sampler2DArray u_texture;

flat in uint layer;
in vec2 texCoord;

out vec4 color;

void main(void){
	color = texture(u_texture, vec3(texCoord, layer));		
}
