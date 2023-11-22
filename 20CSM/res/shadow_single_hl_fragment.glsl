#version 410 core

const int NUM_CASCADES = 10;

struct FogParameters {
	vec3 color;
	float linearStart;
	float linearEnd;
	float density;
	int equation;
};

in vec4 scs[NUM_CASCADES];
in vec4 vPos;
in vec2 v_texCoord;
in vec4 v_color;

uniform sampler2DArray stex;
uniform float far_d[NUM_CASCADES];
uniform mat4 u_shadow[NUM_CASCADES];
uniform vec4 u_color;
uniform vec4 color[4] = vec4[4](	vec4(0.7, 0.7, 1.0, 1.0),
									vec4(0.7, 1.0, 0.7, 1.0),
									vec4(1.0, 0.7, 0.7, 1.0),
									vec4(1.0, 1.0, 1.0, 1.0));
out vec4 outColor;



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

vec4 shadowCoef() {
	int index = 3;
	
	// find the appropriate depth map to look up in based on the depth of this fragment
	if(gl_FragCoord.z < far_d[0])
		index = 0;
	else if(gl_FragCoord.z < far_d[1])
		index = 1;
	else if(gl_FragCoord.z < far_d[2])
		index = 2;
	
	vec4 shadow_coord = u_shadow[index]*vPos;
	float shadow_d = texture(stex, vec3(shadow_coord.xy, index)).r;
	
	float diff = shadow_d - shadow_coord.z;
	return clamp( diff*250.0 + 1.0, 0.0, 1.0)*color[index];
}

void main() {

    const float shadow_ambient = 0.9;
	vec4 color_tex = vec4(1.0); //texture2D(tex, gl_TexCoord[0].st);
	vec4 shadow_coef = shadowCoef();
	outColor = shadow_ambient * shadow_coef * v_color * color_tex + (1.0 - shadow_ambient) * color_tex;
	
	float fogCoordinate = abs(vPos.z / vPos.w);
	outColor = mix(outColor, vec4(u_fogParameter.color, 1.0), getFogFactor(u_fogParameter, fogCoordinate));	
}