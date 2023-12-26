#version 410 core

layout(location = 0) in vec2 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec4 i_color;
layout(location = 3) in uint i_drawState;

flat out uint v_layer;
out vec2 v_texCoord;
out vec4 v_color;

uniform mat4 u_transform = mat4(1.0);

void main(void) {
	gl_Position = u_transform * vec4(i_position, 0.0, 1.0);  
	
    v_texCoord  = i_texCoord;
	v_color = i_color;	
	v_layer = i_drawState;
}
