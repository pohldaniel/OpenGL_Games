#version 410 core

uniform sampler2D u_texture;

in vec2 texCoord;
in vec3 normal;

out vec4 color;

void main(void){
	color = texture2D( u_texture, texCoord );	
	//color = vec4(texCoord, 0.0, 1.0);
}
