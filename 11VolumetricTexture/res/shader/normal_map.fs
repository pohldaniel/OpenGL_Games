#version 410 core

const int MAX_LIGHTS = 4;

struct LightSource{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

struct Material{
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
	float specularShininesse;
};

in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec2 v_texCoord;
in vec3 v_viewDirection;
in vec3 v_lightDirection[MAX_LIGHTS];

out vec4 outColor;

uniform sampler2D u_texture;
uniform sampler2D u_normalMap;
uniform LightSource light[MAX_LIGHTS];
uniform Material material;


void main(void){

	
	vec4 color = texture2D( u_texture, v_texCoord );
	//if(color.a < 0.5) discard;
	
	vec3 E = normalize(v_viewDirection);
	vec3 N = normalize(texture2D( u_normalMap, v_texCoord ).rgb * 2.0 - 1.0) ;
	
	vec3 ambient  = vec3(0.0, 0.0, 0.0);
	vec3 diffuse  = vec3(0.0, 0.0, 0.0);
	vec3 specular = vec3(0.0, 0.0, 0.0);
	
	for(int i = 0; i < 1; i++){
		ambient += light[i].ambient;
			
		float lambert = max(dot(normalize(v_lightDirection[i]),N ), 0.0);
			
		if( lambert > 0.0){
			
			diffuse += light[i].diffuse * lambert;
				
			vec3 V = reflect(normalize(-v_lightDirection[i]),N);
			specular += light[i].specular * pow( max( dot(E,V),0.0 ), 2.0 );
		}	
	}
	
	ambient *= material.ambient;
	diffuse *= material.diffuse;
	specular *= material.specular;
	
	outColor =  color * vec4( (ambient + diffuse ) + specular, 1.0);
}
