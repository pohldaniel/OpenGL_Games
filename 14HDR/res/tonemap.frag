#version 410 core

//precision highp float;

in vec2 texCoord;

uniform sampler2D sceneTex;
uniform sampler2D blurTex;

uniform float blurAmount;
uniform float effectAmount = 0.1;
uniform float exposure;
uniform float gamma = 1.0 / 2.0;

out vec4 color;

float vignette(vec2 pos, float inner, float outer) {
  float r = length(pos);
  r = 1.0 - smoothstep(inner, outer, r);
  return r;
}

vec4 radial(sampler2D tex, vec2 texcoord, int samples, float startScale = 1.0, float scaleMul = 0.9) {
    vec4 c = vec4(0.0, 0.0, 0.0, 0.0);
    float scale = startScale;
    for(int i=0; i < samples; i++) {
        vec2 uv = ((texcoord-0.5)*scale)+0.5;
        vec4 s = texture(tex, uv);
        c += s;
        scale *= scaleMul;
    }
    c /= samples;
    return c;
}

void main(){

	// sum original and blurred image
    vec4 scene = texture(sceneTex, texCoord);
    vec4 blurred = texture(blurTex, texCoord);
	vec4 effect = radial(blurTex, texCoord, 30, 1.0, 0.95);

    vec4 c = mix(scene, blurred, blurAmount);
	
	c += effect * effectAmount;

    // exposure
    c = c * exposure;
    
    // vignette effect
    c *= vignette(texCoord * 2.0 - 1.0, 0.7, 1.5);

    // gamma correction
	c.rgb = pow(c.rgb, vec3(gamma));

    color = c;
}