#version 430 core

const int MAX_LIGHTS = 20;
const float ambientFactor = 0.1;
const float QUADRATIC_ATTEN = 0.05;
const float LINEAR_ATTEN = 0.33;

struct LightStruct {
  bool enabled;
  int type;
  vec4 ambient, diffuse, specular;
  vec3 direction;
  float angle;
  vec3 position;
};

struct Material {
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
uniform Material material;
uniform vec3 u_campos;
uniform bool shadeless = false;

layout(std140, binding = 0) uniform Lights { 
	LightStruct lights[MAX_LIGHTS]; 
};

out vec4 color;

void main(void){
	color = texture2D( u_texture, v_texCoord );
}
