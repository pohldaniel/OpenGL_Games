#version 430 core

uniform sampler2D u_texture;
uniform sampler2D u_irradiance;
uniform sampler2D u_distance;

uniform vec3 u_lightPos = vec3(0.0);
uniform bool u_useDiffuseColor = false;
uniform float u_thikness = 0.1;
uniform float u_minThikness = 0.5;

in vec2 vertTexCoord;
in vec3 vertNormal;

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

float getDepth(float depth, float near, float far){
	float z_ndc = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near + z_ndc * (near - far));	//[near, far]
}

void main(void){

	float attenuation = (1.0f/distance(u_lightPos, vertPosition));
	attenuation *= 10.0f;
	
	vec3 eyeVec      = normalize(eyeDirection);
	vec3 lightVec    = normalize(lightDirection);
	vec3 worldNormal = normalize(vertNormal);
    vec4 dotLN       = vec4(vec3(halfLambert(lightVec, worldNormal) * attenuation), 1.0);
	
	vec3 indirectLightComponent = vec3(materialThickness * max(0, dot(-worldNormal, lightVec)));
	
	indirectLightComponent += vec3(materialThickness * halfLambert(-eyeVec, lightVec));
	indirectLightComponent *= attenuation;
	
	indirectLightComponent.r   *= extinctionCoefficientRed;
	indirectLightComponent.g   *= extinctionCoefficientGreen;
	indirectLightComponent.b   *= extinctionCoefficientBlue;
	
	float zIn  =  textureProj(u_distance, sc).r;
	//float zOut = sc.z /sc.w;	
    //zOut = getDepth(zOut, 1.0, 1024.0);	
	//loat dist = (zOut - zIn);
	float dist = max((distToLight - zIn), u_minThikness) * u_thikness;
	
	indirectLightComponent.rgb *= dist;
	
	vec3 rim = vec3(1.0f - max(0.0f, dot(worldNormal, eyeVec)));
	rim *= rim;
	
	// diffuse color
	if (u_useDiffuseColor)
		dotLN *= textureProj(u_irradiance, sc);
		
	vec4 finalCol = dotLN + vec4(indirectLightComponent, 1.0f);
	rim *= max(0.0f, dot(worldNormal, lightVec)) * specColor;
	
	finalCol.rgb += (rim * rimScalar * attenuation * finalCol.a);
	finalCol.rgb += (blinnPhongSpecular(worldNormal, lightVec, specularPower) * attenuation * specColor * finalCol.a * 0.05f);
	finalCol.rgb *= lightColor;

	color = finalCol * texture2D(u_texture, vertTexCoord);
	//color = vec4(dotLN.r, dotLN.g, dotLN.b, 1.0);
}
