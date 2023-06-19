#version 410 core

uniform sampler2D u_texture;

in vec2 texCoord;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

out vec4 color;

void main(void){
	color = texture2D( u_texture, texCoord );	
}
