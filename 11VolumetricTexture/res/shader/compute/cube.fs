#version 430 core

layout(location = 0) out vec4 color;

in vec3 v_texCoord;

uniform sampler3D u_texture;

void main(void) {   
	color = texture(u_texture, v_texCoord);
	
	//color = vec4(v_texCoord, 1.0);
}
