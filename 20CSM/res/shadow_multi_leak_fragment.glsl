#version 410 core

struct FogParameters {
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	int equation;
};

const int NUM_CASCADES = 10;
// sample offsets
const int nsamples = 8;

in vec4 scs[NUM_CASCADES];
in vec4 vPos;
in vec2 v_texCoord;
in vec4 v_color;

uniform sampler2DArray stex;
uniform sampler2D tex;
uniform float far_d[NUM_CASCADES];
uniform mat4 u_shadow[NUM_CASCADES];
uniform vec4 u_color;
uniform FogParameters u_fogParameter = FogParameters(vec3(0.8, 0.8, 0.8), 16.0, 200.0, 0.4, 0);

uniform vec4 offset[nsamples] = { vec4(0.000000, 0.000000, 0.0, 0.0),
								  vec4(0.079821, 0.165750, 0.0, 0.0),
								  vec4(-0.331500, 0.159642, 0.0, 0.0),
								  vec4(-0.239463, -0.497250, 0.0, 0.0),
								  vec4(0.662999, -0.319284, 0.0, 0.0),
								  vec4(0.399104, 0.828749, 0.0, 0.0),
								  vec4(-0.994499, 0.478925, 0.0, 0.0),
								  vec4(-0.558746, -1.160249, 0.0, 0.0) };

out vec4 outColor;

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

float getOccCoef(vec4 shadow_coord) {
	// get the stored depth
	float shadow_d = texture(stex, shadow_coord.xyz).x;
	
	// get the difference of the stored depth and the distance of this fragment to the light
	float diff = shadow_d - shadow_coord.w;
	
	// smoothen the result a bit, to avoid aliasing at shadow contact point
	return clamp( diff*250.0 + 1.0, 0.0, 1.0);
}


float shadowCoef() {
	const float scale = 2.0/4096.0;
	int index = 3;
	
	// find the appropriate depth map to look up in based on the depth of this fragment
	if(gl_FragCoord.z < far_d[0])
		index = 0;
	else if(gl_FragCoord.z < far_d[1])
		index = 1;
	else if(gl_FragCoord.z < far_d[2])
		index = 2;
	
	vec4 shadow_coord = u_shadow[index]*vPos;
	
	shadow_coord.w = shadow_coord.z;
	
	// tell glsl in which layer to do the look up
	shadow_coord.z = float(index);
	
    // sum shadow samples
	float shadow_coef = getOccCoef(shadow_coord);

	for(int i=1; i<nsamples; i++)
	{
		shadow_coef += getOccCoef(shadow_coord + scale*offset[i]);
	}
	shadow_coef /= nsamples;
	
	return shadow_coef;
}

void main() {
	const float shadow_ambient = 0.9;
	vec4 color_tex = texture2D(tex, v_texCoord);
	float shadow_coef = shadowCoef();
	outColor = shadow_ambient * shadow_coef * v_color * color_tex + (1.0 - shadow_ambient) * color_tex;
	
	float fogCoordinate = abs(vPos.z / vPos.w);
	outColor = mix(outColor, vec4(u_fogParameter.color, 1.0), getFogFactor(u_fogParameter, fogCoordinate));	
}