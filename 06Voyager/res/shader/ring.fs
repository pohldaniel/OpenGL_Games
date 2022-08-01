#version 410 core

uniform float u_radius = 0.7;

in vec2 v_texCoord;

out vec4 color;

float haloRing(vec2 uv, float radius, float thick){

  return clamp(-(abs(length(uv) - radius) * 100.0 / thick) + 0.9, 0.0, 1.0);
}

void main(void){
	
	vec2 uv = v_texCoord * 2.0 - 1.0;
	
	vec4 ringColor = vec4(1.0, 1.0, 1.5, 0.0);
	float intensity = haloRing (uv, u_radius, 16.0);
	color = mix(color, ringColor, intensity);	
}
