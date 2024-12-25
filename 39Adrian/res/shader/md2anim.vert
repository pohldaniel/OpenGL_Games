#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_nextPosition;
layout(location = 4) in vec3 i_nextNormal;

uniform mat4 u_model = mat4(1.0);
uniform mat4 u_view;
uniform mat4 u_projection;
uniform float u_interpolation;

out vec2 v_texCoord;
out vec3 v_normal;

void main()
{	
	vec3 interpolatedPosition = i_position;
	if(u_interpolation >= 0.0f)interpolatedPosition += (i_nextPosition - i_position)*u_interpolation;
  
	gl_Position =  u_projection * u_view * u_model * vec4(interpolatedPosition, 1.0);
	v_texCoord = i_texCoord;
}