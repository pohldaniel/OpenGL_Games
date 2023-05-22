#version 440 core

uniform sampler2D u_texture;
uniform sampler2D u_normalMap;

uniform float invRadius = 1.0;
uniform float alpha = 1.0;

uniform vec4 lightAmbient = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 lightDiffuse = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec4 lightSpecular = vec4(1.0, 1.0, 1.0, 1.0);

uniform vec4 matAmbient = vec4(0.7, 0.7, 0.7, 1.0);
uniform vec4 matDiffuse = vec4(0.8, 0.8, 0.8, 1.0);
uniform vec4 matEmission = vec4(0.0, 0.0, 0.0, 1.0);
uniform vec4 matSpecular = vec4(0.3, 0.3, 0.3, 1.0);
uniform float matShininess = 100.0;

in vec2 texCoord;
in vec4 vertColor;
in vec3 lightVec;
in vec3 eyeVec;
in vec3 normal;

out vec4 color;

void main(void){

	float distSqr = dot(lightVec, lightVec);
	float att = clamp(1.0 - invRadius * sqrt(distSqr), 0.0, 1.0);
	vec3 lVec = lightVec * inversesqrt(distSqr);

	vec3 vVec = normalize(eyeVec);
	
	vec4 base = texture2D(u_texture, texCoord);

	vec3 bump = normalize( texture2D(u_normalMap, texCoord).xyz * 2.0 - 1.0);
	
	vec4 vAmbient = lightAmbient * matAmbient;

	float diffuse = max( dot(lVec, bump), 0.0 );
	
	vec4 vDiffuse = lightDiffuse * matDiffuse * diffuse;	

	float specular = pow(clamp(dot(reflect(-lVec, bump), vVec), 0.0, 1.0), matShininess);
	
	vec4 vSpecular = lightSpecular * matSpecular * specular;	

	color = vec4( vec3(( vAmbient*base + vDiffuse*base + vSpecular)*att) * vertColor.rgb, alpha);		
	
	//color = vSpecular;
}
