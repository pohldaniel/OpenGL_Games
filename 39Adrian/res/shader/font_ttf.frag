#version 410 core

flat in uint v_layer;
in vec2 v_texCoord;
in vec4 v_color;									

out vec4 outColor;

uniform sampler2DArray u_texture;

void main() {
	outColor = v_color * vec4(1.0, 1.0, 1.0, texture(u_texture, vec3(v_texCoord, v_layer)).r);	
} 