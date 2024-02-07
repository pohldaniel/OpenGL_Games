#version 430 core

const int MAX_LIGHTS = 20;
const float ambientFactor = 0.1;
const float QUADRATIC_ATTEN = 0.05;
const float LINEAR_ATTEN = 0.33;

struct Light {
  vec4 ambient, diffuse, specular;
  vec3 position;
  float angle;
  vec3 direction;
  int type;
  bool enabled;
};

struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	float alpha;
};

in vec3 v_color;
in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_position;

uniform sampler2D u_texture;
uniform vec3 u_campos;
uniform bool shadeless = false;

layout(std140, binding = 0) uniform u_lights { 
	Light lights[MAX_LIGHTS]; 
};

layout(std140, binding = 1) uniform u_material {
	Material material;	
};	


out vec4 color;

void main(void){

	vec4 texColor = texture2D( u_texture, v_texCoord);

	color = vec4(0);
	vec3 nWorld = normalize(v_normal);
	
	for (int i = 0; i < MAX_LIGHTS; i++) {
		if (lights[i].enabled) {
			if (lights[i].type == 0) { // Directional
			
				vec3 fragToLight = -lights[i].direction;
				float diffuseFactor = max(dot(fragToLight, nWorld), 0);
				vec3 fragToObserver = normalize(-v_position + u_campos);
				vec3 halfWay = normalize(fragToLight + fragToObserver);
				float specFactor =
					max(pow(dot(halfWay, nWorld), material.shininess), 0) /
					(distance(v_position, u_campos) * LINEAR_ATTEN);
				color +=
					diffuseFactor * lights[i].diffuse * material.diffuse * texColor +
					specFactor * lights[i].specular * material.specular +
					ambientFactor * lights[i].ambient * material.ambient;
				
		    }else if (lights[i].type == 1) { // Point
			
				vec3 fragToLight = normalize(-v_position + lights[i].position);
				float quadraticAtten =
					((pow(distance(v_position, lights[i].position), 2)) *
					 QUADRATIC_ATTEN);
				float diffuseFactor = max(dot(fragToLight, nWorld), 0) / quadraticAtten;
				vec3 fragToObserver = normalize(-v_position + u_campos);
				vec3 halfWay = normalize(fragToLight + fragToObserver);
				float specFactor =
					max(pow(dot(halfWay, nWorld), material.shininess), 0) /
					(quadraticAtten + distance(v_position, u_campos) * LINEAR_ATTEN);
				color +=
					diffuseFactor * lights[i].diffuse * material.diffuse * texColor +
					specFactor * lights[i].specular * material.specular +
					ambientFactor * lights[i].ambient * material.ambient;
					
			}else if (lights[i].type == 2) { // Spot
			
				vec3 fragToLight = normalize(-v_position + lights[i].position);
				float dirToLight = dot(-fragToLight, normalize(lights[i].direction));
				if (dirToLight < cos(radians(lights[i].angle)))
				  continue;
				  
				float quadraticAtten = ((pow(distance(v_position, lights[i].position), 2)) * QUADRATIC_ATTEN);
				float diffuseFactor = max(dot(fragToLight, nWorld), 0) / quadraticAtten;	
				vec3 fragToObserver = normalize(-v_position + u_campos);
				vec3 halfWay = normalize(fragToLight + fragToObserver);	
				float specFactor =
					max(pow(dot(halfWay, nWorld), material.shininess), 0) /
					(quadraticAtten + distance(v_position, u_campos) * LINEAR_ATTEN);
				color +=
				diffuseFactor * lights[i].diffuse * material.diffuse * texColor +
				specFactor * lights[i].specular * material.specular +
				ambientFactor * lights[i].ambient * material.ambient;
			}
		}
	}
	//color = texColor * material.diffuse;
	color.a = texColor.a * material.alpha;
}
