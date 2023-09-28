#version 410 core

uniform sampler2D u_texture;
uniform vec4 u_blendColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

in vec2 texCoord;
in vec3 normal;

out vec4 color;

void main(void){
	color = texture(u_texture, texCoord);	
}
