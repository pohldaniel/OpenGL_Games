#version 430 core

in vec2 v_texCoord;
in vec3 v_normal;

uniform sampler2D u_texture;

out vec4 outColor;

void main() {
	outColor = texture2D(u_texture, v_texCoord);
}