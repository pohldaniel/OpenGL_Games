#version 410 core

uniform sampler2D u_texture;

in vec2 v_texCoord;

out vec4 color;

uniform vec4 u_blendColor = vec4(1.0);

void main(void){
	//color = vec4 (1.0, 0.0, 0.0, 1.0);
	float alpha = texture2D( u_texture, v_texCoord ).a;
	color = u_blendColor * vec4(alpha);
}
