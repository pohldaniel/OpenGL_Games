#version 410 core

layout(location = 0) in vec4 i_base;
layout(location = 1) in vec4 i_posSize;
layout(location = 2) in vec4 i_texPosSize;
layout(location = 3) in uint i_layer;

flat out uint layer;
out vec2 texCoord;

uniform mat4 u_transform = mat4(1.0);

void main(void) {
	gl_Position = u_transform * vec4(i_base.x * i_posSize.z + i_posSize.x, i_base.y * i_posSize.w + i_posSize.y, 0.0, 1.0); 
	texCoord = vec2(i_base.z * i_texPosSize.z + i_texPosSize.x, i_base.w * i_texPosSize.w + i_texPosSize.y);
	layer = i_layer;	
}
