#version 330 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

out vec2 texCoordTiled;
out vec2 texCoord;
out vec4 normal;

uniform float tilingFactor;
uniform mat4 u_transform = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);
uniform vec4 u_plane;

void main(void) {
	gl_ClipDistance[0] = dot(vec4(i_position, 1.0), u_plane);
    gl_Position = u_transform * vec4(i_position, 1.0);
	
	normal = normalize((u_normal * vec4(i_normal, 0.0)));
	normal.w = i_position.y;
	
    texCoordTiled  = i_texCoord * tilingFactor;
	texCoord = i_texCoord;
}