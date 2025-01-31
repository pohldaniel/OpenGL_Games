#version 410 core

flat in uint v_sampler;
flat in uint v_layer;

in vec2 v_texCoord;
in vec4 v_color;

out vec4 outColor;

uniform sampler2DArray u_sampler[8];

void main(void) {
					  
	outColor = v_sampler == 0 ? v_color * vec4(1.0, 1.0, 1.0, texture(u_sampler[v_sampler], vec3(v_texCoord, v_layer)).r) :
								v_color * texture(u_sampler[v_sampler], vec3(v_texCoord, v_layer));
}