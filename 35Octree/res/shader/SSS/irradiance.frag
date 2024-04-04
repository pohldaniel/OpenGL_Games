#version 410 core

uniform sampler2D u_texture;

in vec2 vertTexCoord;
in vec3 vertNormal;
in vec3 lightDirection;

out vec4 color;

float rimScalar	= 30.0f;
float  refractionIndex = 1.66;

float fresnelTransmittance(vec3 N, vec3 V){

	float k = pow(1.0f - max(0.0f, dot(N, V)), 5.0f);	
	float Ft = 1.0 - k + refractionIndex * k;
	return Ft;
}

void main(void){
	vec3 N = normalize(vertNormal);
	vec3 L = normalize(lightDirection);
	vec3 V = L;
	
	float Ft = fresnelTransmittance(N, V);
	float E = Ft * max(0.0f, dot(N, L)) * rimScalar;
	
	vec4 vTexColor = texture2D( u_texture, vertTexCoord );
	
	color = vec4(vTexColor.xyz * E, 1.0);
	//float col = max(0.0f, dot(N, L));
	//color = vec4(col, col, col, 1.0);
}
