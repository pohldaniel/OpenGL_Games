#version 410 core

in vec2 texCoord;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;
in vec3 normalDu;
in vec3 normalDv;

out vec4 color;

void main(void){
	color = vec4 (normal, 1.0);
}
