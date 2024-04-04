#version 410 core

layout(location = 0) out vec4 colorOut;
layout(location = 1) out float distanceOut;

in float vertDistance;

void main(void){
	colorOut = vec4(1.0, 1.0, 0.0, 1.0);
	distanceOut = vertDistance;
}
