#version 410 core

in vec2 v_texCoord;
in vec4 v_color;									

out vec4 outColor;

uniform float u_radius = 5;
uniform vec2 u_dimensions;

void main() {

	vec2 pos = (abs(v_texCoord - 0.5) + 0.5) * u_dimensions;
	vec2 arc_cpt_vec = max(pos - u_dimensions + u_radius, 0.0);
	
	if(length(arc_cpt_vec) > u_radius){
		discard;
	}
	outColor = v_color;
} 