#version 440 core

uniform sampler2DArray u_texture;

in vec2 texCoord;
in vec3 normal;
in vec4 vertColor;
flat in uint vertActive;

out vec4 color;

void main(void){
	color = texture(u_texture, vec3(texCoord, vertActive));	
	//color = vec4(texCoord, 0.0, 1.0);
}
