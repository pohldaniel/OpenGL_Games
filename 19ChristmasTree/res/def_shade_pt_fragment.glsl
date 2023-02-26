#version 410 core

in vec2 texCoord;

in vec3 lightPos;
in vec3 lightColor;

uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D positionTex;

out vec4 outColor;

const float intensity = 128.0;
const float falloff = 512.0;

void main() {
	
	
	vec3 normal = texture2D( normalTex, texCoord).xyz * 2.0 - 1.0;
	vec4 mat = texture2D( materialTex, texCoord);
	vec3 position = texture2D( positionTex, texCoord).xyz;
	
	float dist = distance( lightPos, position);
	vec3 lightDir = normalize( lightPos - position); 
	
	dist = 1.0 / (1.0 + dist * dist * falloff);
	vec3 dif = max( dot( lightDir, normal) * dist * intensity , 0.0) * lightColor;
	
	vec3 spec = vec3(0.0);
	
	//compute specular
	
	if ( mat.w > 0.0f) {
	
		vec3 refl = reflect( normalize(position), normal);
		float s = pow( max( 0.0, dot( refl, lightDir)), mat.w) * dist * intensity;
		spec = s*mix( mat.rgb, vec3(1.0), 0.2) * lightColor;
		dif *= 0.0125f; //reduce diffuse to make it look more metalic
      
	}

	
	
	outColor = vec4( dif*mat.rgb + spec, 1.0);
	
}