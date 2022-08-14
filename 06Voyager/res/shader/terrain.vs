#version 410 core

const int NUM_CASCADES = 10;

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

out vec2 texCoordTiled;
out vec2 texCoord;
out vec4 normal;
out float visibility;
out vec4 scs[NUM_CASCADES];
out float clipSpacePosZ;
flat out uint numCascades;

uniform float tilingFactor;
uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_normal = mat4(1.0);
uniform vec4 u_plane;

uniform mat4 u_projectionShadows[NUM_CASCADES];
uniform mat4 u_viewShadows[NUM_CASCADES];
uniform uint u_numCascades;

const float density = 0.001;
const float gradient = 5.0;

void main(void) {
	gl_ClipDistance[0] = dot(vec4(i_position, 1.0), u_plane);
	vec4 eye = u_modelView * vec4(i_position, 1.0);
    gl_Position = u_projection * eye;
	clipSpacePosZ = gl_Position.z;
	normal = normalize((u_normal * vec4(i_normal, 0.0)));
	normal.w = i_position.y;
	
    texCoordTiled  = i_texCoord * tilingFactor;
	texCoord = i_texCoord;
	
	float distance = length(eye.xyz);
	visibility = exp(-pow(distance * density, gradient));
	visibility = clamp(visibility, 0.0, 1.0);
	
	numCascades = u_numCascades;
	for(uint i = 0; i < numCascades; i++){
		scs[i] = u_projectionShadows[i] * u_viewShadows[i] * vec4(i_position, 1.0);
		scs[i].xyz = scs[i].xyz * 0.5 + vec3(0.5) * scs[i].w;
	}
}