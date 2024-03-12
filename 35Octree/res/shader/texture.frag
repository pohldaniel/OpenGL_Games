#version 430 core

uniform sampler2D u_texture;

in vec3 v_color;
in vec2 v_texCoord;
in vec3 v_normal;
in vec4 vertColor;

out vec4 color;

void main(void){
	color = vertColor * texture2D( u_texture, v_texCoord );
	color.a = 1.0;
}
