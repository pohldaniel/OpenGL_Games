#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

flat out int layer;
out vec2 texCoord;

uniform mat4 u_transform = mat4(1.0);
uniform int u_layer;

void main(void) {
	gl_Position = u_transform * vec4(i_position, 1.0);  
    texCoord  = i_texCoord;
	layer = u_layer;
}
