#version 410 core

uniform sampler2D u_texture;
uniform sampler2D u_depth;

in vec3 v_color;
in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_tangent;
in vec3 v_bitangent;
in vec4 vertColor;

out vec4 color;

void main(void){
	color = texture2D( u_texture, v_texCoord );

	//float rasterized_depth = texture(u_depth, v_texCoord).r;
	//color = vec4(rasterized_depth, rasterized_depth, rasterized_depth, 1.0);
}
