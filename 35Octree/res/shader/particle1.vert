#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in float i_size;
layout(location = 2) in vec4 i_color;
layout(location = 3) in float i_angle;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform float pointMultiplier;

out vec4 vertColor;
out vec2 vertAngle;

void main(void){

	vec4 mvPosition = u_view * u_model * vec4(i_position, 1.0);
	gl_Position = u_projection * mvPosition;
	gl_PointSize = i_size * pointMultiplier / gl_Position.w;
	
	
	vertColor = i_color;
	vertAngle = vec2(cos(i_angle), sin(i_angle));
}