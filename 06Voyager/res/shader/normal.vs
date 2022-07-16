#version 410 core

const int MAX_LIGHTS = 4;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;

out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec3 v_lightDirection[MAX_LIGHTS];
out vec3 v_viewDirection;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal;
uniform mat4 u_modelViewLight;

uniform vec3 u_lightPos[MAX_LIGHTS];

void main(void){

	v_normal = normalize((u_normal * vec4(i_normal, 0.0)).xyz);
	v_tangent = normalize((u_normal * vec4(i_tangent, 0.0)).xyz);
	v_bitangent = normalize((u_normal * vec4(i_bitangent, 0.0)).xyz);

	mat3 TBN = mat3(v_tangent.x, v_bitangent.x, v_normal.x,
					v_tangent.y, v_bitangent.y, v_normal.y,
					v_tangent.z, v_bitangent.z, v_normal.z);

	vec4 vertexInCamSpace = u_modelView * vec4(i_position, 1.0);

	v_viewDirection = - TBN * vertexInCamSpace.xyz;

	for(int i = 0; i < 1; i++){	
		v_lightDirection[i] =  TBN *((u_modelViewLight * vec4(u_lightPos[i], 1.0)).xyz - vertexInCamSpace.xyz);	 
	}

	gl_Position = u_projection * vertexInCamSpace;
	v_texCoord = i_texCoord;
}