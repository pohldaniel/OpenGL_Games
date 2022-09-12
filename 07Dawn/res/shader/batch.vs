#version 410 core

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in uint i_layer;

flat out uint layer;

out vec2 texCoord;

uniform mat4 u_transform = mat4(1.0);

void main(void) {
	gl_Position = u_transform * vec4(i_position, 0.0, 1.0);  
    texCoord  = i_texCoord;
	layer = i_layer;
}
