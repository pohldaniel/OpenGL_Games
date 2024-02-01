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
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	float alpha;
	
};	

out vec4 color;

void main(void){
	color = texture2D( u_texture, v_texCoord ) * diffuse;
	color.a *= alpha;
}
