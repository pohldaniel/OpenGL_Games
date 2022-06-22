#version 410 core

struct Material {
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	int shininess;
};

uniform sampler2D u_texture;
uniform Material material;

in vec3 v_color;
in vec2 v_texCoord;

out vec4 color;

void main(void){
	color = vec4 (material.diffuse.xyz, 1.0) * texture(u_texture, v_texCoord);	
}
