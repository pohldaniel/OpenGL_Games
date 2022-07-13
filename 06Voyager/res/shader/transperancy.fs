#version 410 core

uniform sampler2D u_texture;

in vec3 v_color;
in vec2 v_texCoord;

out vec4 outColor;

void main(void){
	vec4 color = texture2D( u_texture, v_texCoord );
	//if(color.a < 0.5) discard;
	
	outColor = color;
}
