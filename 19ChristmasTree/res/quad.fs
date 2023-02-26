#version 330

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D u_texture;

void main(void) {
	outColor = texture(u_texture, texCoord);
	
	//outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
