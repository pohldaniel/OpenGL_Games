#version 410 core

in vec2 texCoord;


uniform sampler2D normalTex;
uniform sampler2D materialTex;
uniform sampler2D positionTex;
uniform samplerCube cubeTex;

out vec4 outColor;

void main() {
	
	
	vec3 normal = texture2D( normalTex, texCoord).xyz * 2.0 - 1.0;
	vec4 mat = texture2D( materialTex, texCoord);
	vec3 position = texture2D( positionTex, texCoord).xyz;
	
	vec3 spec = vec3(0.0);
	
	//only apply reflections, if material is reflective
	if ( mat.w > 0.0f) {
	
		vec3 refl = reflect( normalize(position), normalize(normal));
		
		// bias the reflection to blur it for rougher surfaces, scale by 1/4 to bring it into range
		vec3 env = texture( cubeTex, refl, 5.0 - log(mat.w)).rgb * 0.25;
		
		spec = env * mat.rgb;
	}
	
	outColor = vec4( spec, 1.0);
}