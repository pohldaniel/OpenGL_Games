#version 430 core


in vec2 uv;
in vec4 color;

uniform vec4 uColor;
uniform sampler2D uTexture;

out vec4 colorOut;

void main() {

	vec4 texel = texture2D(uTexture, uv);
	vec4 result = texel * uColor * color;
	colorOut = result;
}