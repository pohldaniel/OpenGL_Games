#version 330

flat in int layer;
in vec2 texCoord;
out vec4 outColor;

uniform sampler2DArray u_texture;

void main(void) {
	outColor = texture(u_texture, vec3(texCoord, layer));
}
