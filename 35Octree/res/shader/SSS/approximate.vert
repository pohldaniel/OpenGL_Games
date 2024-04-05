#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);
uniform vec3 u_camPos = vec3(0.0); 

uniform mat4 u_lightView = mat4(1.0);
uniform mat4 u_lightViewProj = mat4(1.0);
uniform mat4 u_lightTexcoord = mat4(1.0);
uniform vec3 u_lightPos = vec3(0.0);

out vec2 vertTexCoord;
out vec3 vertNormal;
 

out vec4 sc;
out vec3 lightDirection;
out vec3 eyeDirection;
out vec3 vertPosition;
out float distToLight;

float calculateDistToLight(vec4 position){
	return length((u_lightView * position).xyz);
}

void main(void){
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
	vertPosition = (u_model * vec4(i_position, 1.0)).xyz;
	lightDirection = vertPosition - u_lightPos ;
	eyeDirection = u_camPos - vertPosition;
	distToLight = calculateDistToLight(vec4(i_position, 1.0));
	
	vertTexCoord = i_texCoord;
	vertNormal = mat3(u_normal) * i_normal;
	
	vec3 shrinkedPos = i_position - 0.003 * normalize(i_normal);
	sc = u_lightTexcoord * vec4(shrinkedPos, 1.0);
}