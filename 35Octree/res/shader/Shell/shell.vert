#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;

uniform mat4 u_model;
uniform mat4 u_normal;

out vec3 vertNormal;
out vec3 vertTangent;
out vec2 vertTexCoord;

void main(){
	gl_Position = u_model * vec4(i_position, 1.0);
	vertNormal = mat3(u_normal) * i_normal;
	vertTangent = mat3(u_normal) * i_tangent;
	vertTexCoord = vec2(4.0 * i_texCoord.x, 2.0 * i_texCoord.y + i_texCoord.x);
}