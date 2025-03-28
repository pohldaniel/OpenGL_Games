#version 430 core

#define SEPARATE_SPECULARS 1

layout (location = 0) out vec4  outColor;
layout (location = 1) out vec4  outSpecularColor;
layout (location = 2) out float outDepth;
layout (location = 3) out vec2  outVelocity;
layout (location = 4) out vec4  outTransmittance;

uniform float near = 1.0; 
uniform float far  = 100.0; 

uniform sampler2D u_depthMap[4];
uniform sampler2D u_albedo;
uniform sampler2D u_normal;
uniform sampler2D u_specular;
uniform sampler2D u_beckmann;
uniform sampler2DShadow u_shadowMap[4];

uniform float bumpiness = 0.89;
uniform float specularIntensity = 0.85;
uniform float specularRoughness = 0.1;
uniform float specularFresnel = 0.81;
uniform float weight = 1.0;
uniform float u_strength = 3.0;

uniform vec3 lightPosition[4];
uniform vec3 lightDirection[4];
uniform vec3 lightColor[4];
uniform float lightFalloffStart[4];

uniform float lightFarPlane = 100.0;
uniform float lightAttenuation = 1.0 / 128.0;
uniform float lightFalloffWidth = 0.05;
uniform float lightBias = -0.01;

uniform bool tangentSpace = false;
uniform bool specularAOMap = false;
uniform bool u_shadow = true;

uniform float sssWidth = 200.0;
uniform float translucency = 0.0;
uniform float ambient = 0.12;
uniform float translucencyStrength = 5.0;

uniform mat4 u_projectionShadow[4];
uniform mat4 u_viewShadow[4];
uniform mat4 u_projectionShadowBias[4];
uniform mat4 u_normalMatrix;

in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec2 v_texCoord;
in vec3 v_worldPosition;
in vec3 v_view;
in vec4 sv_position;
in vec4 v_currPosition;
in vec4 v_prevPosition;
in vec4 vertColor;

in vec4 sc[4];

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

vec3 BumpMap2(sampler2D normalTex, vec2 texcoord) {
    vec3 bump;
    bump.xy = texture2D( u_normal, v_texCoord ).gr * 2.0 - 1.0;
    bump.z = sqrt(1.0 - bump.x * bump.x - bump.y * bump.y);
    return normalize(bump);
}

float Fresnel(vec3 _half, vec3 view, float f0) {
    float base = 1.0 - dot(view, _half);
    float exponential = pow(base, 5.0);
    return exponential + f0 * (1.0 - exponential);
}

float SpecularKSK(sampler2D beckmannTex, vec3 normal, vec3 light, vec3 view, float roughness) {
    vec3 _half = view + light;
    vec3 halfn = normalize(_half);

    float ndotl = max(dot(normal, light), 0.0);
    float ndoth = max(dot(normal, halfn), 0.0);

    float ph = pow(2.0 * texture2D(beckmannTex, vec2(ndoth, roughness)).r, 10.0);
    float f = mix(0.25, Fresnel(halfn, view, 0.028), specularFresnel);
    float ksk = max(ph * f / dot(_half, _half), 0.0);

    return ndotl * ksk;    
}

float Shadow(vec3 worldPosition, int i) {
    vec4 shadowPosition = u_projectionShadowBias[i] * u_viewShadow[i] * vec4(worldPosition, 1.0);
	return textureProj(u_shadowMap[i], shadowPosition);
    
	
    //shadowPosition.z += lightBias;	
	//shadowPosition.xyz /= shadowPosition.w;
	//return texture(u_shadowMap[i], shadowPosition.xyz);
}

float ShadowPCF(vec3 worldPosition, int i, int samples, float width) {
	vec4 shadowPosition = u_projectionShadowBias[i] * u_viewShadow[i] * vec4(worldPosition, 1.0);
    shadowPosition.xy /= shadowPosition.w;
    shadowPosition.z += lightBias;	
	
	ivec2 size = textureSize(u_shadowMap[i], 0);
	
	float shadow = 0.0;
    float offset = (samples - 1.0) / 2.0;
	
	for (float x = -offset; x <= offset; x += 1.0) {
        for (float y = -offset; y <= offset; y += 1.0) {
            vec2 pos = shadowPosition.xy + width * vec2(x, y) / float(size.x);
            shadow += texture(u_shadowMap[i], vec3(pos, shadowPosition.z / shadowPosition.w));
        }
    }
    shadow /= samples * samples;
    return shadow;
}

float getDepthPassSpaceZ(float z_ndc, float near, float far){
	z_ndc = z_ndc * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near + z_ndc * (near - far));
}

vec3 SSSSTransmittance( float translucency, float sssWidth, vec3 worldNormal, vec3 light, sampler2D depthMap, vec4 shadowPos){

	float scale = translucencyStrength * (1.0 /u_strength) * sssWidth * (1.0 - translucency);

	float zIn =  texture2DProj(depthMap, shadowPos).r;
	float zOut = shadowPos.z/shadowPos.w;

	zIn = getDepthPassSpaceZ(zIn, 1.0, 100.0);
	zOut = getDepthPassSpaceZ(zOut, 1.0, 100.0);

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
	vec3 normal;
	
	if(tangentSpace){
		vec3 t = normalize(v_tangent);
		vec3 b = normalize(v_bitangent);
		vec3 n = normalize(v_normal);
		
		mat3 TBN = mat3(t.x, t.y, t.z,
						b.x, b.y, b.z,
						n.x, n.y, n.z);

		normal = TBN *  mix(vec3(0.0, 0.0, 1.0), BumpMap(u_normal, v_texCoord), bumpiness); 
	
	}else{
		vec3 objSpaceNormal = normalize(texture2D( u_normal, v_texCoord ).rgb - 0.5);
		objSpaceNormal.rgb = objSpaceNormal.rbg; objSpaceNormal.bg *= -1; // normal conversion
		vec3 worldSpaceNormal = mat3(u_normalMatrix) * objSpaceNormal;
		normal = mix(v_normal, worldSpaceNormal, bumpiness);
	}

     
	vec3 view = normalize(v_view);
   
	vec4 albedo = texture2D(u_albedo, v_texCoord);
	albedo.a = 1.0;
	albedo.rgb = albedo.rgb * vertColor.rgb * (1.0 /weight);
	vec3 specularAO;
	
	if(specularAOMap)
		specularAO = texture2D( u_specular, v_texCoord ).rgb;
	else
		specularAO = vec3(0.6, 0.2, 0.9);
		
	float occlusion = specularAO.b;
    float intensity = specularAO.r * specularIntensity;
    float roughness = (specularAO.g / 0.3) * specularRoughness;
  
  
	// Initialize the output:
	outColor = vec4(0.0, 0.0, 0.0, 0.0);
	outSpecularColor.rgb = vec3(0.0, 0.0, 0.0);
   
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
            vec3 f2 = albedo.rgb * f1;

			 // Calculate the diffuse and specular lighting:
            vec3 diffuse = vec3(clamp(dot(light, normal) , 0.0, 1.0));
			 
			float specular = intensity * SpecularKSK(u_beckmann, normal, light, view, roughness);
            //float shadow = Shadow(v_worldPosition, i);
			float shadow = ShadowPCF(v_worldPosition, i, 3, 1.0);
			if(!u_shadow)
				shadow = 1.0;
			#ifdef SEPARATE_SPECULARS
            outColor.rgb += shadow * f2 * diffuse;
            outSpecularColor.rgb += shadow * f1 * specular;
            #else
            outColor.rgb += shadow * (f2 * diffuse + f1 * specular);
            #endif
			
			vec3 transmittance = SSSSTransmittance(translucency, sssWidth, normalize(v_normal), light, u_depthMap[i], sc[i]);
			
			outTransmittance.rgb += transmittance;			
			outColor.rgb += f2 * albedo.a * transmittance * weight;
		}
	}

    outColor.rgb += occlusion * ambient * albedo.rgb ;
 
	// Store the SSS strength:
	outSpecularColor.a = albedo.a * u_strength;

    // Store the depth value:
    outDepth = gl_FragCoord.z;
	
	// Calculate velocity in non-homogeneous projection space:	
	vec2 currPosition = v_currPosition.xy/v_currPosition.w;
	vec2 prevPosition = v_prevPosition.xy/v_prevPosition.w;
	

    outVelocity = (currPosition - prevPosition);
	
	// Compress the velocity for storing it in a 8-bit render target:
    outColor.a = sqrt(5.0 * length(outVelocity));
	
	//outColor = vec4(normal, 1.0);
}
