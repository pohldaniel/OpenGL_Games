#version 430 core

in vec2 v_texCoord;
in vec4 vertPosition;

out vec4 colorOut;

uniform sampler2D u_diffuse;
uniform mat4 u_view;
uniform float u_resolutionY;

void main(){ 
	vec4 worldPosCentre = vec4(0,0.05,0,1);
	vec4 eyeSpaceCentre = u_view * worldPosCentre;
	vec3 toCentre = eyeSpaceCentre.xyz - vertPosition.xyz;
	float distToCentreSquared = pow(toCentre.x,2) + pow(toCentre.y,2) + pow(toCentre.z,2);
	colorOut = texture2D(u_diffuse,v_texCoord);

	colorOut.b /= clamp(distToCentreSquared*2,0,1);
	colorOut.g /= clamp(distToCentreSquared*2,0,1);

	float maxDist = 6f;
	vec4 a = colorOut;
	vec4 b = vec4(0.2,0.2,0.2,1);
	float k = min((distToCentreSquared/maxDist),1);
	colorOut.r = a.r + (b.r - a.r) * k;
	colorOut.g = a.g + (b.g - a.g) * k;
	colorOut.b = a.b + (b.b - a.b) * k;
	
	colorOut.a *= 1- (gl_FragCoord.y/u_resolutionY);
}