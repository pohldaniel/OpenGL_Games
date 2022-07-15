#version 410 core

const uint NUM_CASCADES = 10;

in vec2 texCoord;
in vec4 normal;
in vec4 scs[NUM_CASCADES];
in float clipSpacePosZ;
flat in uint numCascades;

layout(location = 0) out vec4 outColor;

uniform sampler2D u_texture;
uniform sampler2DArray u_shadowMaps;
uniform float u_cascadeEndClipSpace[NUM_CASCADES];
uniform bool u_debug = false;

float CalcShadowFactor(uint cascadeIndex, vec4 sc){
    vec3 ndc = (sc.xyz/sc.w);
	float depth = texture(u_shadowMaps, vec3(ndc.xy, cascadeIndex)).r;

	return ndc.z > depth  ? 0.1 : 1.0;
}

void main(void) {

	float ShadowFactor = 0.0;
	vec4 CascadeIndicator = vec4(0.0, 0.0, 0.0, 0.0);

	vec4 color = texture(u_texture, texCoord);

    for(uint i = 0; i < numCascades; i++){
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
			else if (i == 4)
                CascadeIndicator = vec4(1.0, 1.0, 1.0, 0.0);
            break;
        }
    }
	if(!u_debug) CascadeIndicator = vec4(0.0, 0.0, 0.0, 0.0);
	outColor = (color  + CascadeIndicator)* ShadowFactor;
}