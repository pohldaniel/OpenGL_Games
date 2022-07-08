#version 330 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;


out vec2 texCoord;
out vec4 normal;
out vec4 sc;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_modelView = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);

uniform mat4 u_projectionShadow;
uniform mat4 u_viewShadow;

void main(void) {

    gl_Position = u_projection * u_modelView * vec4(i_position, 1.0);	
	normal = normalize((u_normal * vec4(i_normal, 0.0)));
	
	texCoord = i_texCoord;
	
	
	sc = u_projectionShadow * u_viewShadow * vec4(i_position, 1.0);
	sc.xyz = sc.xyz * 0.5 + vec3(0.5) * sc.w;
}