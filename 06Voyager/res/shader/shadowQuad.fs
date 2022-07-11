#version 330 core

const int NUM_CASCADES = 4;

in vec2 texCoord;
in vec4 normal;
in vec4 sc;
in vec4 scs[NUM_CASCADES];
in float clipSpacePosZ;

layout(location = 0) out vec4 outColor;

uniform sampler2D u_texture;
uniform sampler2D u_shadowMap;
uniform sampler2D u_shadowMaps[NUM_CASCADES];
uniform float u_cascadeEndClipSpace[NUM_CASCADES];

float CalcShadowFactor(int cascadeIndex, vec4 sc){
    vec3 ndc = (sc.xyz/sc.w);
	float depth = texture(u_shadowMaps[cascadeIndex], ndc.xy).r;

	return ndc.z > depth  ? 0.0 : 1.0;
}

void main(void) {

	float ShadowFactor = 0.0;
	vec4 CascadeIndicator = vec4(0.0, 0.0, 0.0, 0.0);
	//ShadowFactor = CalcShadowFactor(2, scs[2]);

	vec4 color = texture(u_texture, texCoord);

	
    for (int i = 0 ; i < NUM_CASCADES ; i++) {
		if (clipSpacePosZ <= u_cascadeEndClipSpace[i]) {
            ShadowFactor = CalcShadowFactor(i, scs[i]);		

			if (i == 0) 
                CascadeIndicator = vec4(1.0, 0.0, 0.0, 0.0);
            else if (i == 1)
                CascadeIndicator = vec4(0.0, 1.0, 0.0, 0.0);
            else if (i == 2)
                CascadeIndicator = vec4(0.0, 0.0, 1.0, 0.0);
			else if (i == 3)
                CascadeIndicator = vec4(0.0, 1.0, 1.0, 0.0);
            break;
        }
    }
	
	
	//vec3 ndc = (sc.xyz/sc.w);
	//float depth = texture(u_shadowMap, ndc.xy).r;
	//float shadow = ndc.z > depth  ? 0.0 : 1.0;

	//vec3 ndc = (scs[1].xyz/scs[1].w);
	//float depth = texture(u_shadowMaps[1], ndc.xy).r;
	//float shadow = ndc.z > depth  ? 0.0 : 1.0;

	

	outColor = (color  + CascadeIndicator)* ShadowFactor;
}