#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;

uniform mat4 u_lightViewProj = mat4(1.0);
uniform mat4 u_lightView = mat4(1.0);
uniform vec3 u_lightPos = vec3(0.0);

out float vertDistance;

float calculateDistToLight(vec4 position){
	return length((u_lightView * position).xyz);
}

void main(void){
	gl_Position = u_lightViewProj * vec4(i_position, 1.0);
	vertDistance = calculateDistToLight(vec4(i_position, 1.0));
}