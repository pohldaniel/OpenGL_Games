//https://www.mbsoftworks.sk/tutorials/opengl4/020-fog/

#version 410 core

const int NUM_CASCADES = 10;

in vec4 scs[NUM_CASCADES];
in vec4 vPos;
in vec2 v_texCoord;
in vec4 v_color;

uniform sampler2DArray stex;
uniform sampler2D tex;
uniform float far_d[NUM_CASCADES];
uniform vec4 u_color;

out vec4 outColor;

struct FogParameters {
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	int equation;
};

uniform FogParameters u_fogParameter = FogParameters(vec3(0.8, 0.8, 0.8), 16.0, 200.0, 0.4, 0);

float getFogFactor(FogParameters params, float fogCoordinate) {
	float result = 0.0;
	if(params.equation == 0) {
		float fogLength = params.linearEnd - params.linearStart;
		result = (params.linearEnd - fogCoordinate) / fogLength;
	}else if(params.equation == 1) {
	
		result = exp(-params.density * fogCoordinate);
	}else if(params.equation == 2) {
	
		result = exp(-pow(params.density * fogCoordinate, 2.0));
	}
	
	result = 1.0 - clamp(result, 0.0, 1.0);
	return result;
}

float shadowCoef(){
	int index = 3;
	
	// find the appropriate depth map to look up in based on the depth of this fragment
	if(gl_FragCoord.z < far_d[0])
		index = 0;
	else if(gl_FragCoord.z < far_d[1])
		index = 1;
	else if(gl_FragCoord.z < far_d[2])
		index = 2;
	
	// transform this fragment's position from view space to scaled light clip space
	// such that the xy coordinates are in [0;1]
	// note there is no need to divide by w for othogonal light sources
    vec3 ndc = (scs[index].xyz/scs[index].w);
	
	// get the stored depth
	float shadow_d = texture(stex, vec3(ndc.xy, index)).r;
	
	// get the difference of the stored depth and the distance of this fragment to the light
	float diff = shadow_d - ndc.z;
	
	// smoothen the result a bit, to avoid aliasing at shadow contact point
	return clamp( diff*250.0 + 1.0, 0.0, 1.0);
}

void main(){
	const float shadow_ambient = 0.9;
	float shadow_coef = shadowCoef();
	vec4 color_tex = texture2D(tex, v_texCoord);
	outColor = shadow_ambient * shadow_coef * v_color * color_tex + (1.0 - shadow_ambient) * color_tex;

	float fogCoordinate = abs(vPos.z / vPos.w);
	outColor = mix(outColor, vec4(u_fogParameter.color, 1.0), getFogFactor(u_fogParameter, fogCoordinate));	
}