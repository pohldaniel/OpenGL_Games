#version 330

flat in float alpha;
in vec2 texCoord;
out vec4 outColor;

uniform sampler2D u_texture;

void main(void) {
	outColor = texture(u_texture, texCoord);
	outColor.a = outColor.a == 0.0f ? 0.0f : alpha;
}
