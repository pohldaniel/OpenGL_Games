#version 410 core

flat in int v_layer;
in vec2 v_texCoord;
in vec4 v_color;									

uniform sampler2DArray u_texture;
uniform float u_fade = 0.0;

out vec4 outColor;

const vec3 white = vec3(1.0);

void main() {
	outColor = v_color * texture(u_texture, vec3(v_texCoord, v_layer));
	outColor = mix(outColor, vec4(white, outColor.a), u_fade);
}