#version 430 core

uniform sampler2D u_texture;
uniform sampler2D u_irradiance;
uniform sampler2D u_distance;

uniform vec3 u_lightPos = vec3(0.0);
uniform bool m_useDiffuseColor = false;

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec4 vertColor;

in vec4 sc;
in vec3 lightDirection;
in vec3 eyeDirection;
in vec3 vertPosition;
in float distToLight;

out vec4 color;

float materialThickness	= 0.6f;
float extinctionCoefficientRed = 0.80f;
float extinctionCoefficientBlue = 0.12f;
float extinctionCoefficientGreen = 0.20f;
vec3 specColor = vec3(0.9f, 0.9f, 1.0f);
float specularPower = 1.0f;
float rimScalar	= 30.0f;
vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

float halfLambert(vec3 a, vec3 b){
	float product = dot(a, b);
	product *= 0.5;
	product += 0.5;
	return product;
}

float blinnPhongSpecular(vec3 normalVec, vec3 lightVec, float specPower){
	vec3 halfAngle = normalize(normalVec + lightVec);
	return pow(clamp(dot(normalVec, halfAngle), 0.0, 1.0), specPower);
}

void main(void){

	float attenuation = (1.0f/distance(u_lightPos, vertPosition));
	attenuation *= 10.0f;
	
	vec3 eyeVec      = normalize(eyeDirection);
	vec3 lightVec    = normalize(lightDirection);
	vec3 worldNormal = normalize(v_normal);
	vec4 dotLN      = vec4(halfLambert(lightVec, worldNormal) * attenuation);
	
	vec3 indirectLightComponent = vec3(materialThickness * max(0, dot(-worldNormal, lightVec)));
	
	indirectLightComponent += vec3(materialThickness * halfLambert(-eyeVec, lightVec));
	indirectLightComponent *= attenuation;
	
	indirectLightComponent.r   *= extinctionCoefficientRed;
	indirectLightComponent.g   *= extinctionCoefficientGreen;
	indirectLightComponent.b   *= extinctionCoefficientBlue;
	
	vec4 ndc = sc/sc.w;
	float dist = distToLight - texture2D( u_distance, ndc.xy).r;
	indirectLightComponent.rgb *= dist;
	
	vec3 rim = vec3(1.0f - max(0.0f, dot(worldNormal, eyeVec)));
	rim *= rim;
	
	// diffuse color
	if (m_useDiffuseColor)
		dotLN *= texture2D(u_irradiance, ndc.xy);
		
	vec4 finalCol = dotLN + vec4(indirectLightComponent, 1.0f);
	rim *= max(0.0f, dot(worldNormal, lightVec)) * specColor;
	
	finalCol.rgb += (rim * rimScalar * attenuation * finalCol.a);
	finalCol.rgb += (blinnPhongSpecular(worldNormal, lightVec, specularPower) * attenuation * specColor * finalCol.a * 0.05f);
	finalCol.rgb *= lightColor;

	color = finalCol * texture2D(u_texture, v_texCoord);
	//color = vec4(dist, dist, dist, 1.0);
}
