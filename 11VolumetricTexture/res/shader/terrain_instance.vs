#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in mat4 i_model;

out vec2 texCoordTiled;
out vec2 texCoord;
out vec4 normal;

uniform float tilingFactor;
uniform mat4 u_transform = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);

void main(void) {
    gl_Position =  u_transform *i_model * vec4(i_position, 1.0);
	
	normal = normalize((u_normal * vec4(i_normal, 0.0)));
	normal.w = i_position.y;
	
    texCoordTiled  = i_texCoord * tilingFactor;
	texCoord = i_texCoord;
}