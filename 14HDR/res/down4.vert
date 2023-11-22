#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

uniform vec2 twoTexelSize;

out vec2 texCoord[4];

void main() {
	gl_Position = vec4(i_position, 1.0);

	texCoord[0] = i_texCoord;
    texCoord[1] = i_texCoord + vec2(twoTexelSize.x, 0);
    texCoord[2] = i_texCoord + vec2(twoTexelSize.x, twoTexelSize.y);
    texCoord[3] = i_texCoord + vec2(0, twoTexelSize.y);
}