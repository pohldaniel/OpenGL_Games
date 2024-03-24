#version 430 core

uniform sampler2D u_texture;
uniform sampler2D u_depth;

in vec4 vertColor;
in vec2 vertAngle;
in float vertZ;
in float vertSize;

uniform float u_near;
uniform float u_far;
uniform vec2 u_resolution;
uniform float u_fallOff;

out vec4 outColor;

float getDepth(vec2 uv, float near, float far){
	float z_final = texture2D( u_depth, uv ).x;
	return ( near * far ) / ( ( far - near ) * z_final - far );
}

void main(void){
	vec2 coords = (gl_PointCoord - 0.5) * mat2(vertAngle.x, vertAngle.y, -vertAngle.y, vertAngle.x) + 0.5;
	outColor = vertColor;
	

	vec2 screenCoords = gl_FragCoord.xy / u_resolution;
	float sceneDepth = getDepth(screenCoords, u_near, u_far);
	float curDepth = vertZ;
	float falloffRange = vertSize * u_fallOff;
	float diff = clamp(curDepth - sceneDepth / falloffRange, 0.0, 1.0);
	diff = smoothstep(0.0, 1.0, diff);
	
	outColor.a = diff;	
	outColor.rgb *= outColor.a;
	
	outColor *= texture2D(u_texture, coords); 
}
