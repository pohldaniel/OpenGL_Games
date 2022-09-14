#version 410 core

flat in uint layer;
in vec2 texCoord;
out vec4 outColor;											

uniform sampler2D u_texture;
uniform vec4 u_blendColor = vec4(1.0);

void main() {
	outColor = u_blendColor * vec4(1.0, 1.0, 1.0, texture(u_texture, texCoord).r);
} 