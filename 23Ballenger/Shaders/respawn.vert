#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoord;
layout (location = 2) in vec3 i_normal;
layout (location = 5) in mat4 i_model;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_normal;

layout (std140, binding = 0) uniform u_color{
	vec4 color[8];
};

layout (std140, binding = 1) uniform u_activate{
	bool activate[8];
};

out vec2 texCoord;
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;
out vec4 vertColor;

flat out uint vertActive;

void main(void){
	gl_Position = u_projection * u_view * i_model * vec4(i_position, 1.0);
   
	texCoord = i_texCoord;  
	normal = normalize(mat3(u_normal * i_model) * i_normal);

	vertColor = color[gl_InstanceID];
	vertActive = uint(activate[gl_InstanceID]);
}