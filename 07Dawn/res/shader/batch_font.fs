#version 410 core

flat in uint v_layer;
in vec2 v_texCoord;
in vec4 v_color;

out vec4 outColor;

uniform sampler2DArray u_interface;
uniform sampler2DArray u_sprite;
uniform sampler2D u_font;



uniform vec4 u_blendColor = vec4(1.0);

void main(void) {
	outColor = v_layer == 2 ? v_color * vec4(1.0, 1.0, 1.0, texture(u_font, v_texCoord).r) : (v_layer == 1 ? v_color * texture(u_sprite, vec3(v_texCoord, 0)) : v_color * texture(u_interface, vec3(v_texCoord, 0)));
}