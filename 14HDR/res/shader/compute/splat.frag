#version 430 core

in vec3 v_texCoord;

out vec4 c;

uniform vec3 centre;
uniform float radius;
uniform vec4 color;

void main(){
	
	float d = length(v_texCoord - centre);
    c = (1.0 - smoothstep(0.0, radius, d)) * color;
}