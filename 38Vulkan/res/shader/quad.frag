#version 410 core

in vec2 v_texCoord;
in vec4 v_color;

uniform sampler2D u_texture;

out vec4 outColor;

void main(void){
	outColor = texture2D( u_texture, v_texCoord ) * v_color;
}
