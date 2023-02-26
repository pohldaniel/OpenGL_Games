#version 410 core

//all these are in eye-space

in vec2 texCoord;
in vec3 normal;
in vec3 tangentS;
in vec3 tangentT;
in vec3 position;


uniform sampler2D baseTex;
uniform sampler2D normalTex;

layout(location = 0) out vec4 positionOut;
layout(location = 1) out vec4 normalOut;
layout(location = 2) out vec4 baseOut;

void main() {

	
	vec3 bumpNormal = texture2D( normalTex, texCoord).xyz * 2.0 - 1.0;
	vec4 base = texture2D( baseTex, texCoord);
	
	if (base.a < 0.75)
	    discard;
	
	bumpNormal = bumpNormal.x * tangentS + bumpNormal.y * tangentT + bumpNormal.z * normal;
	
	bumpNormal = faceforward( bumpNormal, normalize(position), bumpNormal);
	
	positionOut = vec4( position, 0.0);
	normalOut = vec4( bumpNormal * 0.5 + 0.5, base.a);
	baseOut = vec4( base.rgb, 0.0);
}