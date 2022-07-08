#version 330 core

in vec2 texCoord;
in vec4 normal;
in vec4 sc;

layout(location = 0) out vec4 outColor;

uniform sampler2D u_texture;
uniform sampler2D u_shadowMap;


void main(void) {

	vec3 ndc = (sc.xyz/sc.w);
	float depth = texture(u_shadowMap, ndc.xy).r;
	
	vec4 color = texture(u_texture, texCoord);
	
	
	float shadow = ndc.z > depth  ? 0.0 : 1.0;

	outColor = color * shadow;
}