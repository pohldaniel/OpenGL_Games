#version 430 core

const int MAX_JOINTS = 96;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 5) in vec4 i_jointWeights;
layout(location = 6) in ivec4 i_jointIds;


layout(std140, binding = 3) uniform u_skinMatrices{
    mat4 skinMatrices[MAX_JOINTS];
};

uniform mat4 u_model = mat4(1.0);
uniform mat4 u_view;
uniform mat4 u_projection;
uniform vec4 u_color = vec4(1.0); 

out vec4 v_worldPos;
out vec2 v_texCoord;
out vec3 v_normal;
out vec4 vertColor; 

mat4 GetWorldMatrix(){
    return skinMatrices[i_jointIds.x] * i_jointWeights.x + skinMatrices[i_jointIds.y] * i_jointWeights.y +
           skinMatrices[i_jointIds.z] * i_jointWeights.z + skinMatrices[i_jointIds.w] * i_jointWeights.w;
}

void main(){
	mat4 world = u_model * GetWorldMatrix();   	
	v_worldPos = world * vec4(i_position, 1.0);   
	v_texCoord = i_texCoord;
	v_normal = (world * vec4(i_normal, 0.0)).xyz; 	
	vertColor = u_color;
	gl_Position = u_projection * u_view * v_worldPos;
}