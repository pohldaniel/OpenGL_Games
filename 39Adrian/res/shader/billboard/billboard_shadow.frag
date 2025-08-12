#version 410 core

layout(location = 0) out vec4 color;
layout(location = 1) out float depth;

uniform sampler2D u_texture;

in vec2 texCoord; 

void main() {
	color = texture(u_texture, texCoord);
	depth = gl_FragCoord.z;
}