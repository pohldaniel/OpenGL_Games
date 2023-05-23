#version 440 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoord;
layout (location = 2) in vec3 i_normal;
layout (location = 5) in mat4 i_model;
layout (location = 9) in vec4 i_color;
layout (location = 10) in mat4 i_modelDyn;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model;
uniform mat4 u_normal;
uniform vec3 u_lightPos = vec3(0.0, 10.0, 0.0);

out vec2 texCoord;
out vec3 lightVec; 
out vec3 eyeVec;
out vec3 tangent;
out vec3 normal;
out vec4 vertColor;

void main(void){
	gl_Position = u_projection * u_view * i_model * i_modelDyn * u_model* vec4(i_position, 1.0);
   
	texCoord = i_texCoord;  
	vertColor = i_color;
	
	vec3 c1 = cross(i_normal, vec3(0.0, 0.0, 1.0)); 
	vec3 c2 = cross(i_normal, vec3(0.0, 1.0, 0.0)); 
	if(length(c1)>length(c2)) tangent = c1;	
	else tangent = c2;	

	normal = normalize(mat3(u_normal * i_model) * i_normal);
	vec3 n = normalize(mat3(u_normal * i_model) * i_normal);
	vec3 t = normalize(mat3(u_normal * i_model) * tangent);
	vec3 b = cross(n, t);
	
	vec3 vVertex = vec3(u_view * i_model * vec4(i_position, 1.0));
	vec3 tmpVec = u_lightPos - vVertex;

	lightVec.x = dot(tmpVec, t);
	lightVec.y = dot(tmpVec, b);
	lightVec.z = dot(tmpVec, n);

	tmpVec = -vVertex;
	eyeVec.x = dot(tmpVec, t);
	eyeVec.y = dot(tmpVec, b);
	eyeVec.z = dot(tmpVec, n);
}