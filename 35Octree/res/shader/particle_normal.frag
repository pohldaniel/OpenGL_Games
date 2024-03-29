#version 430 core


in vec2 uv;
in vec4 color;

uniform vec4 uColor;
uniform sampler2D uTexture;

void main() {

	vec4 texel = texture2D(uTexture, uv);
	vec4 result = texel * uColor * color;
	gl_FragColor = result;
}