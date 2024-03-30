#version 430 core

uniform sampler2D u_texture;

in vec3 v_color;
in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec4 vertColor;

out vec4 color;

void main(void){
	color = vertColor * texture2D( u_texture, v_texCoord );
	
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}
