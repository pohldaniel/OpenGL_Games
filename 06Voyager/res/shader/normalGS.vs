#version 410 core


layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;

out vec3 normal;
out vec3 tangent;
out vec3 bitangent;


uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;


void main(void){

	normal = normalize((u_normal * vec4(i_normal, 0.0)).xyz);
	tangent = normalize((u_normal * vec4(i_tangent, 0.0)).xyz);
	bitangent = normalize((u_normal * vec4(i_bitangent, 0.0)).xyz);

	

	gl_Position = u_modelView * vec4(i_position, 1.0);

}