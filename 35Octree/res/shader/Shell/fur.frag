#version 410 core

layout (location = 0) out vec4 color;

uniform sampler2D fur_texture;
uniform vec4 fur_color = vec4(0.8, 0.8, 0.9, 1.0);

in GS_FS_VERTEX{
	vec3 normal;
	vec2 texCoord;
	flat float fur_strength;
} fragment_in;

void main(void){
	vec4 rgba = texture(fur_texture, fragment_in.texCoord);
	float t = rgba.a;
	t *= fragment_in.fur_strength;
	color = fur_color * vec4(1.0, 1.0, 1.0, t);
	
	//color = vec4(1.0, 0.0, 0.0, 1.0);
}