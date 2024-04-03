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


out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec4 vertColor; 

out vec4 sc;
out vec3 lightDirection;
out vec3 eyeDirection;
out vec3 vertPosition;
out float distToLight;

float calculateDistToLight(vec4 position){
	return length(u_lightView * position);
}

void main(void){

	vertPosition = (u_model * vec4(i_position, 1.0)).xyz;
	lightDirection = u_lightPos - vertPosition;
	eyeDirection = u_camPos - vertPosition;
	distToLight = length(lightDirection);

	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
	v_texCoord = i_texCoord;
	v_normal = mat3(u_normal) * i_normal;
	v_tangent = i_tangent;
	v_bitangent = i_bitangent;
	vertColor = u_color;
   
	sc = u_lightTexcoord * vec4(i_position, 1.0);
}