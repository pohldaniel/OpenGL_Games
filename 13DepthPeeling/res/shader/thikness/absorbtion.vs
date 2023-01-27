#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;

uniform mat4 u_projectionShadow;
uniform mat4 u_viewShadow;

out vec4 sc;

void main(){
    gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
	
	vec3 shrinkedPos = i_position - 0.005 * normalize(i_normal);
	
	sc = u_projectionShadow  * u_viewShadow * u_model * vec4(shrinkedPos, 1.0);
	sc.xyz = sc.xyz * 0.5 + vec3(0.5) * sc.w;
}