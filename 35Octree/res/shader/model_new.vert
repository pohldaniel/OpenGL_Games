#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform vec4 u_color = vec4(1.0); 
uniform vec3 u_lightPos;

out vec2 v_texCoord;
out vec3 vertLightDirection;

void main(){
	gl_Position = u_projection * u_view * u_model *  vec4(i_position, 1.0);
	
	mat3 normalMat = mat3(u_model);
	vec3 normal = normalize(normalMat * i_normal);
	vec3 tangent = normalize(normalMat * i_tangent);
	vec3 bitangent = normalize(normalMat * i_bitangent);
	mat3 TBN = mat3(tangent.x, bitangent.x, normal.x,
					tangent.y, bitangent.y, normal.y,
					tangent.z, bitangent.z, normal.z);
		
	v_texCoord = i_texCoord;	
	vertLightDirection =  TBN *(u_lightPos - (u_model * vec4(i_position, 1.0)).xyz);	 
}