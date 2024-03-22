#version 430 core

out vec4 outColor;

uniform sampler2D u_texture;

in vec4 vertColor;
in vec2 vertAngle;
in float vertBlend;

void main(void){
	vec2 coords = (gl_PointCoord - 0.5) * mat2(vertAngle.x, vertAngle.y, -vertAngle.y, vertAngle.x) + 0.5;
	outColor = texture2D(u_texture, coords) * vertColor;
	outColor.rgb *= vertColor.a;
	outColor.a *= vertBlend;	
}