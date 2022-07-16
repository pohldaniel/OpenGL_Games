#version 410 core

in vec3 v_texCoord;

out vec4 color;

uniform samplerCube u_day;
uniform samplerCube u_nigth;
uniform float u_blendFactor = 0.3;
uniform vec4 fogColor;

const float lowerLimit = 0.0;
const float upperLimit = 1000.0;

void main(void){
	vec4 day = texture(u_day, v_texCoord);
	vec4 nigth = texture(u_nigth, v_texCoord);
	vec4 finalColor = mix(day, nigth, u_blendFactor);
	
	float factor = (v_texCoord.y - lowerLimit) / (upperLimit - lowerLimit);
	factor = clamp(factor, 0.0, 1.0);
	
	color =	finalColor;
	//color = mix(fogColor, finalColor, factor);	
}