#version 410 core

layout(location = 0) out vec4 color;

in vec2 texCoord;

void main(void){
	color = vec4(texCoord, 0.0, 1.0);	
}
