#version 410 core

flat in uint v_sampler;
flat in uint v_layer;

in vec2 v_texCoord;
in vec4 v_color;

out vec4 outColor;

uniform sampler2DArray u_nearest;
uniform sampler2DArray u_linear;
uniform sampler2DArray u_font;

uniform vec4 u_blendColor = vec4(1.0);

void main(void) {
	outColor = v_sampler == 2 ? v_color * vec4(1.0, 1.0, 1.0, texture(u_font, vec3(v_texCoord, v_layer)).r) :
			   v_sampler == 1 ? v_color * texture(u_linear, vec3(v_texCoord, v_layer))
							  : v_color * texture(u_nearest, vec3(v_texCoord, v_layer));
}