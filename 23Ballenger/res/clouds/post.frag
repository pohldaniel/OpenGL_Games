#version 410 core
#extension GL_ARB_shading_language_include : require
#include "/header.glsl"

const float TIME_OFFSET = 0.0;
const float TIME_SPEED = 1.0;

out vec4 color;

in vec2 vUvs;

uniform sampler2D frameTexture;
uniform vec2 frameResolution;
uniform vec2 resolution;
uniform float time;

vec3 oklabToRGB(vec3 c);
vec3 saturate3(vec3 x);
vec3 vignette(vec2 uvs);

void main(void){
 
  vec2 pixelCoords = (vUvs - 0.5) * resolution;

  float curTime = time*TIME_SPEED + TIME_OFFSET;

  vec4 colour = texture(frameTexture, vUvs);

#ifdef USE_OKLAB
  colour.xyz = oklabToRGB(colour.xyz);
#endif

  // Vignette
  colour.xyz *= vignette(vUvs);
  colour.xyz = pow(saturate3(colour.xyz), vec3(1.0 / 2.2));

  color = vec4(colour.xyz, 1.0);
}
