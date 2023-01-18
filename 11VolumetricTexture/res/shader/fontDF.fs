#version 410 core

uniform sampler2D u_texture;

in vec2 v_texCoord;

out vec4 color;

uniform vec4 u_blendColor = vec4(1.0);

const float width = 0.5;
const float edge = 0.1;

const float borderWidth = 0.4;
const float borderEdge = 0.5;

const vec2 offset = vec2(0.003, -0.003);

const vec4 outlineColor = vec4(0.0, 1.0, 1.0, 1.0);

void main(void){
	
	float distance = 1.0 - texture2D( u_texture, v_texCoord ).a;
	float alpha = 1.0 - smoothstep(width, width + edge, distance);
	
	float distance2 = 1.0 - texture2D( u_texture, v_texCoord + offset).a;
	float outlineAlpha = 1.0 - smoothstep(borderWidth, borderWidth + borderEdge, distance2);
	
	float overallAlpha = alpha + (1.0 - alpha) * outlineAlpha;
	vec4 overallColor = mix(outlineColor, u_blendColor, alpha / overallAlpha);
	
	color = overallColor * vec4(overallAlpha);
}
