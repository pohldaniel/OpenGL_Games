#version 410 core

uniform sampler2D u_texture;
uniform sampler2D u_shadowMap;

in vec2 texCoord;
in vec3 normal;
in vec4 vertColor;
in vec4 sc;

out vec4 color;

void main(void){

	vec4 shad = sc/sc.w;
	   float dist = texture2D(u_shadowMap, shad.st).z;
	   float shadow = 1.0;
	   if(sc.w > 0)
		   shadow = (dist < shad.z) ? 0.5 : 1.0;

	color = shadow * vertColor * texture2D( u_texture, texCoord );	
	color.a = 1.0;
}
