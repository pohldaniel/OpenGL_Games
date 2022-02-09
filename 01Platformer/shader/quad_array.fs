#version 330

in vec2 fTexCoord;
out vec4 outColor;

uniform sampler2DArray u_texture;
uniform int layer;

void main(void) {
	outColor = texture(u_texture, vec3(fTexCoord, layer));
}
