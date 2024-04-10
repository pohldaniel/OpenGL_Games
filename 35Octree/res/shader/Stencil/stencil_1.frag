#version 410 core

uniform sampler2D u_baseTexture;
uniform sampler2D u_blendTexture;

in vec2 vertTexCoord;
in vec4 vertColor; 

out vec4 outColor;

void main(void){
	outColor = vertColor * mix(texture(u_baseTexture, vertTexCoord), texture(u_blendTexture, vertTexCoord), 0.5);

}
