#version 410 core

uniform sampler2D u_texture;
uniform sampler2D u_gradient;

in vec3 v_color;
in vec2 v_texCoord;

out vec4 color;

uniform float dissolveAmount = 0.5;
uniform float edgeThickness = 0.1;
uniform vec4 edgeColor = vec4(1.0, 0.0, 0.0, 1.0);
uniform float noiseTiling = 1.0;

void main(void){
	
	vec4 originalTexture = texture(u_texture, v_texCoord);
	vec4 dissolveNoise = texture(u_gradient, v_texCoord * noiseTiling);
	float remappedDissolve = dissolveAmount * (1.01  + edgeThickness) - edgeThickness;
	vec4 step1 = step(remappedDissolve, dissolveNoise);
	vec4 step2 = step(remappedDissolve + edgeThickness, dissolveNoise);
	vec4 edge = step1 - step2;
	
	edge.a = originalTexture.a;
	vec4 edgeColorArea = edge * edgeColor;
	originalTexture.a *= step1.r;
	vec4 combinedColor = mix(originalTexture, edgeColorArea, edge.r);
	color = combinedColor;
	
}
