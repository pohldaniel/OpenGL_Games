#version 410 core

flat in uint layer;
in vec2 texCoord;
out vec4 outColor;

uniform sampler2DArray u_texture;
uniform vec4 u_blendColor = vec4(1.0);

void main(void) {
	outColor = u_blendColor * texture(u_texture, vec3(texCoord, layer));
}