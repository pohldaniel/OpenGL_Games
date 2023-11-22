#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 2) in vec2 i_texcoords;
layout(location = 1) in vec3 i_normal;
layout(location = 3) in vec3 i_tangentS;
layout(location = 4) in vec3 i_tangentT;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;

//all these are in eye-space
out vec3 normal;
out vec3 tangentS;
out vec3 tangentT;
out vec3 position;
out vec2 texCoord;

void main() {

	vec4 fragPos = u_modelView * vec4(i_position, 1.0);
	gl_Position = u_projection * fragPos;
	position = fragPos.xyz;

	//transform tangents and normal
	normal =  mat3(u_normal) * i_normal;
	tangentS = mat3(u_normal) * i_tangentS;
	tangentT = mat3(u_normal) * i_tangentT;
	
	texCoord = i_texcoords;	
}