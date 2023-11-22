#version 410 core

in vec2 texCoord;

uniform sampler2D fullSize;
uniform sampler2D downSampled;

uniform float exposure = 0.25;
uniform float gamma = 2.2;
uniform float blur = 0.4;

out vec4 outColor;

// vignetting effect (makes corners of image darker)
float vignette(vec2 pos, float inner, float outer)
{
  float r = length(pos);
  r = 1.0 - smoothstep(inner, outer, r);
  return r;
}

void main() {
	vec3 large = texture2D( fullSize, texCoord).rgb;
	vec3 small = texture2D( downSampled, texCoord).rgb;
	
	vec3 col = mix( large, small, blur) * exposure * vignette( texCoord *2.0 - 1.0, 0.7, 1.5);
	
	
	outColor = vec4( pow( col, vec3( 1.0 / 2.2)), 1.0);
}