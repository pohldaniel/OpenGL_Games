#version 410 core

in vec3 v_texCoord;

out vec4 color;

uniform samplerCube u_day;
uniform samplerCube u_nigth;
uniform float u_blendFactor = 0.3;

void main(void){
	vec4 day = texture(u_day, v_texCoord);
	vec4 nigth = texture(u_nigth, v_texCoord);
	vec4 finalColor = mix(day, nigth, u_blendFactor);
	
	color = finalColor;	
}