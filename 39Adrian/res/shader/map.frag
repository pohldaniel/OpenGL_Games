#version 410 core

uniform sampler2D u_texture;
uniform sampler2DShadow u_shadowMap;
uniform vec4 u_blendColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);

in vec2 texCoord;
in vec3 normal;
in vec4 sc;

out vec4 color;

void main(void){

	//vec4 shad = sc / sc.w;
	//float dist = texture2D(u_shadowMap, shad.xy).r;
	
	float dist = textureProj(u_shadowMap, sc);
	
	float shadow = 1.0;
	if(sc.w > 0)
		shadow = (dist < sc.z) ? 0.5 : 1.0;

	color = shadow * u_blendColor * texture2D( u_texture, texCoord );	
	color.a = 1.0;
}
