#version 410 core

const int NUM_CASCADES = 10;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal = mat4(1.0);
uniform vec4 u_color;
uniform vec3 u_lightPos = vec3(0.0, 0.0, 1.0);
uniform mat4 u_viewProjectionShadows[NUM_CASCADES];
uniform uint u_numCascades = 4;

out vec2 v_texCoord;
out vec3 v_normal;
out vec4 scs[NUM_CASCADES];
out vec4 vPos;
out vec4 v_color;
flat out uint numCascades;

vec4 diffLight = vec4(1.0);

void main(void) {
	
	vPos = u_modelView * vec4(i_position, 1.0);
    gl_Position = u_projection * vPos;
	
	v_normal = mat3(u_normal) * i_normal;
	v_texCoord = i_texCoord;
	v_texCoord.y = 1.0 - v_texCoord.y;
	v_color = u_color * diffLight * vec4(max(dot(v_normal, u_lightPos), 0.0));

	numCascades = u_numCascades;
	for(uint i = 0; i < numCascades; i++){
		scs[i] = u_viewProjectionShadows[i]  * vec4(i_position, 1.0);
		scs[i].xyz = scs[i].xyz * 0.5 + vec3(0.5) * scs[i].w;
	}
}