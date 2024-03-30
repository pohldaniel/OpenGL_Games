#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in float i_size;
layout(location = 2) in vec4 i_color;
layout(location = 3) in float i_angle;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform float u_pointMultiplier;

out vec4 vertColor;
out vec2 vertAngle;
flat out float vertZ;
out float vertSize;

void main(void){
  gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
  gl_PointSize = i_size * u_pointMultiplier / gl_Position.w;
	
	
  vertColor = i_color;
  vertAngle = vec2(cos(i_angle), sin(i_angle));
  
  //inside frustum [near, far]
  vertZ = gl_Position.z;
  vertSize = i_size;
}