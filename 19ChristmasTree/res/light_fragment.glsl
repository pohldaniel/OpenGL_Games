#version 410 core

in vec3 oColor;
in vec2 coord;

out vec4 outColor;

void main() {

    //make the light round
	if ( dot( coord, coord) > 1.0)
	    discard;
	
	outColor = vec4( oColor, 1.0);
}