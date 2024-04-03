#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_invView = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);
uniform vec4 u_color = vec4(1.0); 


uniform mat4 u_lightView = mat4(1.0);
uniform mat4 u_lightViewProj = mat4(1.0);
uniform mat4 u_lightTexcoord = mat4(1.0);
uniform vec3 u_lightPos = vec3(0.0);
uniform vec3 u_camPos = vec3(0.0);

void main(void){

}