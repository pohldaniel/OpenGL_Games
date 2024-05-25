#version 430 core

struct Material{
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	float shininess;
	float alpha;
};

layout(std140, binding = 1) uniform u_material {
	Material material;	
};

uniform sampler2D u_texture;


in vec3 v_color;
in vec2 v_texCoord;
in vec3 v_normal;

out vec4 color;

void main(void){
	color = material.diffuse;
}