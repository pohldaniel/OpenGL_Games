#version 430 core

layout(location = 0) in vec3 i_position;

uniform mat4 u_lightViewProj = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform vec3 u_lightPos;
uniform vec3 u_camPos;

out vec4 vertDepthCoord;
out vec3 vertLightDir;
out vec3 vertViewDir;

void main() {

  gl_Position = u_model * vec4(i_position, 1.0);
  
  vertDepthCoord = u_lightViewProj * gl_Position;
  vertLightDir = u_lightPos - gl_Position.xyz;
  vertViewDir = u_camPos - gl_Position.xyz;
}
