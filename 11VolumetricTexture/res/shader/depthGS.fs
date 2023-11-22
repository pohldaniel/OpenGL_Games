#version 410 core

in vec2 v_texCoord;

//out vec4 outColor;

uniform sampler2D u_texture;

void main(void){

	float alpha = texture2D( u_texture, v_texCoord ).a;
	if(alpha < 0.5) discard;
   
	//outColor = vec4(alpha);
}