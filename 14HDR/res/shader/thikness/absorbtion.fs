#version 430 core

uniform sampler2D Sampler;
uniform vec3 DiffuseMaterial;

in vec4 sc;

void main() {

	vec4 ndc = sc/sc.w;
	float thickness =  abs(texture2D(Sampler, ndc.xy ).r);
   
    if (thickness <= 0.0){
        discard;
    }
	
    float sigma = 30.0;
    float fresnel = 1.0 - texture2D(Sampler, ndc.xy ).g;
    float intensity = fresnel * exp(-sigma * thickness);
    gl_FragColor = vec4(intensity * DiffuseMaterial, 1.0);
}