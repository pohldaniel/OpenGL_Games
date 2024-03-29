#version 430 core

layout(location = 0) in vec4 aPosition;
layout(location = 1) in vec2 aUV0;
layout(location = 2) in vec3 aColor0;
layout(location = 3) in float aSize;
layout(location = 4) in float aAlpha;

uniform mat4 uMVP;
uniform mat4 uV_inv;

out vec2 uv;
out vec4 color;

void main() {
	uv = aUV0.xy;
	color = vec4(aColor0.rgb,aAlpha);
	vec2 newUV = (aUV0 - vec2(0.5)) * aSize;
	vec3 offset = newUV.x * uV_inv[0].xyz + newUV.y * uV_inv[1].xyz;
	gl_Position = uMVP * ( aPosition + vec4(offset , 0.0) );
}