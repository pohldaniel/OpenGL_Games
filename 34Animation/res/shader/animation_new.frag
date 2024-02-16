#version 430 core

in vec2 v_texCoord;
in vec3 v_normal;
in vec4 vertColor;

uniform sampler2D u_texture;

out vec4 outColor;

void main() {
	outColor = vertColor * texture2D(u_texture, v_texCoord);
	outColor.a = 1.0;
}