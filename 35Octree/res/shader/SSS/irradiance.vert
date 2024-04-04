#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);
uniform mat4 u_lightViewProj = mat4(1.0);
uniform mat4 u_lightViewInverse = mat4(1.0);
uniform mat4 u_lightView = mat4(1.0);
uniform vec3 u_lightPos = vec3(0.0);


out vec2 vertTexCoord;
out vec3 vertNormal;
out vec4 vertColor;
out vec3 lightDirection;

void main(void){
	vertTexCoord = i_texCoord;
	vertNormal = mat3(u_lightViewInverse) * i_normal;
	lightDirection = (u_model * vec4(i_position, 1.0)).xyz - u_lightPos ;
	gl_Position = u_lightViewProj * vec4(i_position, 1.0);
}