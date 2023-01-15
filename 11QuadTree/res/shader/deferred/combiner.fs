#version 330

layout(location = 0) out vec4 FS_OUT_Albedo;
layout(location = 1) out vec4 FS_OUT_Normal;

in vec2 texCoord;

uniform sampler2D s_Albedo_decal;
uniform sampler2D s_Normal_decal;

void main(void) {
    FS_OUT_Albedo = texture(s_Albedo_decal, texCoord);
	FS_OUT_Normal = texture(s_Normal_decal, texCoord);
	
	//vec4 color = texture(s_Albedo_decal, texCoord);
	//vec4 normal = texture(s_Normal_decal, texCoord);
	
	//FS_OUT_Albedo = vec4(color.a);
}
