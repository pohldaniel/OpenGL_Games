#version 330

flat in int layer;
in vec2 texCoord;
out vec4 outColor;

uniform sampler2DArray u_texture;


void main(void) {
	float depth = texture(u_texture, vec3(texCoord, layer)).r;
	float distance = depth;

	outColor = vec4(distance);
}
