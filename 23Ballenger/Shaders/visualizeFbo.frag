#version 410 core

in vec2 TexCoords;

out vec4 color;

uniform sampler2D fboTex;

void main(){
	vec4 col = texture(fboTex, TexCoords);
	color = col;
}  