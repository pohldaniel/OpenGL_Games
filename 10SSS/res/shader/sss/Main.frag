#version 410 core

layout (location = 0) out vec4  outColor;
layout (location = 1) out vec4  outSpecularColor;
layout (location = 2) out float outDepth;
layout (location = 3) out vec2  outVelocity;
layout (location = 4) out vec4  outTransmittance;

uniform float near = 1.0; 
uniform float far  = 100.0; 

uniform sampler2D u_shadowMap[4];
uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform sampler2D u_beckmann;

uniform float bumpiness = 0.89;
uniform float specularIntensity = 0.46;
uniform float specularRoughness = 0.3;
uniform float specularFresnel = 0.81;

uniform vec3 lightPosition[4];
uniform vec3 lightDirection[4];
uniform vec3 lightColor[4];
uniform float lightFalloffStart[4];

uniform float lightFarPlane = 100.0;
uniform float lightAttenuation = 1.0 / 128.0;
uniform float lightFalloffWidth = 0.05;
uniform float lightBias = -0.01;

uniform bool sssEnabled = true;
uniform bool translucencyEnabled = true;
uniform bool separateSpeculars = false;

uniform float sssWidth = 200.0;
uniform float translucency = 0.0;
uniform float ambient = 0.12;

uniform mat4 u_projectionShadow[4];
uniform mat4 u_viewShadow[4];
uniform mat4 u_projectionShadowBias[4];

varying vec3 v_normal;
varying vec3 v_tangent;
varying vec3 v_bitangent;
varying vec2 v_texCoord;
varying vec3 v_worldPosition;
varying vec3 v_view;
varying vec4 sv_position;
varying vec4 v_currPosition;
varying vec4 v_prevPosition;

varying vec4 sc[4];

const vec3 weights[5] = vec3[5](
	vec3(0.153845, 0.159644, 0.01224),
	vec3(0.019712, 0.010854, 0.383008),
	vec3(0.048265, 0.402794, 0.022724),
	vec3(0.42143, 0.021578, 0.161395),
	vec3(0.009353, 0.050785, 0.051702)
);

const vec3 variances[5] = vec3[5](
	vec3(0.243373, 0.19885, 0.000128),
	vec3(0.001579, 0.000128, 1.76679),
	vec3(0.016459, 2.29636, 0.001403),
	vec3(3.10841, 0.001473, 0.164507),
	vec3(0.000131, 0.014631, 0.013066)
);

vec3 BumpMap(sampler2D normalTex, vec2 texcoord) {
    vec3 bump;
    bump.xyz = texture2D( u_normal, v_texCoord ).rgb * 2.0 - 1.0;
    return normalize(bump);
}

float Fresnel(vec3 half, vec3 view, float f0) {
    float base = 1.0 - dot(view, half);
    float exponential = pow(base, 5.0);
    return exponential + f0 * (1.0 - exponential);
}

float SpecularKSK(sampler2D beckmannTex, vec3 normal, vec3 light, vec3 view, float roughness) {
    vec3 half = view + light;
    vec3 halfn = normalize(half);

    float ndotl = max(dot(normal, light), 0.0);
    float ndoth = max(dot(normal, halfn), 0.0);

    float ph = pow(2.0 * texture2D(beckmannTex, vec2(ndoth, roughness)).r, 10.0);
    float f = mix(0.25, Fresnel(halfn, view, 0.028), specularFresnel);
    float ksk = max(ph * f / dot(half, half), 0.0);

    return ndotl * ksk;    
}

float getDepthPassSpaceZ(float zWC, float near, float far){
	return (near * far) / (far + zWC * (near - far));	//[near, far]
}

vec3 SSSSTransmittance( float translucency, float sssWidth, vec3 worldNormal, vec3 light, sampler2D shadowMap, vec4 shadowPos){

	float scale = sssWidth * (1.0 - translucency);

	//vec4 shrinkedPos = vec4(v_worldPosition - 0.005 * worldNormal, 1.0);
	//vec4 shadowPosition = u_projectionShadowBias[0]   * u_viewShadow[0] * shrinkedPos;
	//vec3 ndc = (shadowPosition.xyz/shadowPosition.w);
	
	//float zIn = texture2D(shadowMap, ndc.xy ).r;
	//float zOut = ndc.z;
	
	vec4 ndc = shadowPos/shadowPos.w;
	float zIn =  texture2D(shadowMap, ndc.xy ).r;
	float zOut = ndc.z;

	zIn = getDepthPassSpaceZ(zIn, 1.0, 100.0);
	zOut = getDepthPassSpaceZ(zOut, 1.0, 100.0);

	//float d =  scale * texture2D(shadowMap, ndc.xy ).r * 100.0;
	float d = scale *( zOut - zIn); 

	float dd = -d * d;

	vec3 profile = 	 weights[0] * exp(dd / variances[0]) +
					 weights[1] * exp(dd / variances[1]) +
					 weights[2] * exp(dd / variances[2]) +
					 weights[3] * exp(dd / variances[3]) +
					 weights[4] * exp(dd / variances[4]);

	/** 
	* Using the profile, we finally approximate the transmitted lighting from
	* the back of the object:
	*/	 
	return profile * clamp(0.3 + dot(light, -worldNormal), 0, 1);
   
    //return vec3(d);
	
}

void main(){

	vec3 t = normalize(v_tangent);
	vec3 b = normalize(v_bitangent);
	vec3 n = normalize(v_normal);
	
	//not transposed to push view- and light- vector to normal space
	mat3 TBN = mat3(t.x, b.x, n.x,
   				 t.y, b.y, n.y,
   				 t.z, b.z, n.z);

	vec3 normal = TBN * BumpMap(u_normal, v_texCoord); 
 
     
	vec3 view = normalize(v_view);
   
	vec4 albedo = texture2D(u_albedo, v_texCoord);
	vec3 specularAO = vec3(0.6, 0.2, 0.9);

    float occlusion = specularAO.b;
    float intensity = specularAO.r * specularIntensity;
    float roughness = (specularAO.g / 0.3) * specularRoughness;
  
  
	// Initialize the output:
	outColor = vec4(0.0, 0.0, 0.0, 0.0);
	outSpecularColor = vec4(0.0, 0.0, 0.0, 0.0);
   
	for(int i = 0; i < 4; i++){
		vec3 light = lightPosition[i] - v_worldPosition;
		float dist = length(light);
        light /= dist;
		
		float spot = dot(lightDirection[i], -light);
   
        if (spot > lightFalloffStart[i]) {
			float curve = min(pow(dist / lightFarPlane, 6.0), 1.0);
			float attenuation = mix(1.0 / (1.0 + lightAttenuation * dist * dist), 0.0, curve);
			
			// And the spot light falloff:
			spot = clamp((spot - lightFalloffStart[i]) / lightFalloffWidth, 0.0, 1.0);
			 
			 // Calculate some terms we will use later on:
			 vec3 f1 = lightColor[i] * attenuation * spot;
             vec3 f2 = albedo.rgb * f1 * 2.0;

			 // Calculate the diffuse and specular lighting:
             vec3 diffuse = vec3(clamp(dot(light, normal) , 0.0, 1.0));
			 
			float specular = intensity * SpecularKSK(u_beckmann, normal, light, view, roughness);
            float shadow = 1.0;
			
			outSpecularColor.rgb += shadow * f1 * specular;
			
			outColor.rgb += shadow * (f2 * diffuse + f1 * specular);
			vec3 transmittance = SSSSTransmittance(translucency, sssWidth, normalize(v_normal), light, u_shadowMap[i], sc[i]);
			
			outTransmittance.rgb += transmittance;			
			outColor.rgb += f2 * albedo.a * transmittance;
		}
	}

    outColor.rgb += occlusion * ambient * albedo.rgb ;
 
	// Store the SSS strength:
    outSpecularColor.a = albedo.a;
	
    // Store the depth value:
    outDepth = gl_FragCoord.z;
	
	// Calculate velocity in non-homogeneous projection space:	
	vec2 currPosition = v_currPosition.xy/v_currPosition.w;
	vec2 prevPosition = v_prevPosition.xy/v_prevPosition.w;
	

    outVelocity = (currPosition - prevPosition);
	
	// Compress the velocity for storing it in a 8-bit render target:
    outColor.a = sqrt(5.0 * length(outVelocity));
}
