#version 330

flat in int layer;
in vec2 texCoord;
in vec4 vertColor;

out vec4 outColor;

uniform sampler2DArray u_texture;

void main(void) {
	outColor = vertColor * texture(u_texture, vec3(texCoord, layer));
	
	//outColor = vec4(1.0, 0.0, 0.0, 1.0);
}
