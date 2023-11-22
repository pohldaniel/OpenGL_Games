#version 410 core

in vec2 texCoord;

uniform sampler2D normalTex;
uniform sampler2D positionTex;
uniform sampler2D materialTex;

out vec4 outColor;

void main() {
    //directional light, coming from over the right shoulder in eye-space
	vec3 light =  vec3( 0.57735);
	
	vec4 normal = texture2D( normalTex, texCoord);
	vec3 position = texture2D( positionTex, texCoord).xyz;
	vec4 mat = texture2D( materialTex, texCoord);
	vec3 bumpNormal = normal.xyz * 2.0 - 1.0;
	float factor = length(bumpNormal);
	
	float d = dot( light, bumpNormal) * 1.0;
	
	d = max( 0.0, d) + 0.2;
	d = min( normal.w, d);
	
	vec3 spec = vec3(0.0);
	
	if ( mat.w > 0.0f) {
	
		vec3 refl = reflect( normalize(position), bumpNormal);
		float s = pow( max( 0.0, dot( refl, light)), mat.w);
		spec = s*mix( mat.rgb, vec3(1.0), 0.2);
		d *= 0.0125f; //reduce diffuse to make it look more metalic

	}
	
	outColor = vec4( d*mat.rgb + spec, 1.0);
	
	//outColor = vec4( 1.0, 0.0, 0.0, 1.0);
}