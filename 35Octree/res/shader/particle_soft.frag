#version 430 core


in vec2 uv;
in vec4 color;

uniform vec4 uColor;
uniform sampler2D uTexture;

uniform vec2 uScreenSize;
uniform vec4 u_FMinusN_FPlusN_FTimesNTimes2_N;

uniform sampler2D uDepthTextureComponent24;

float DepthToWorld_optimized(float z){
	float z_aux = z * 2.0 - 1.0;
	return u_FMinusN_FPlusN_FTimesNTimes2_N.z / (u_FMinusN_FPlusN_FTimesNTimes2_N.y - z_aux * u_FMinusN_FPlusN_FTimesNTimes2_N.x);
}

void main() {

	vec4 texel = texture2D(uTexture, uv);
	vec4 result = texel * uColor * color;

	float framebuffer_depth = texture2D(uDepthTextureComponent24, (gl_FragCoord.xy / uScreenSize) ).x;          
	framebuffer_depth = DepthToWorld_optimized(framebuffer_depth);   
	
	float particle_depth = DepthToWorld_optimized(gl_FragCoord.z);
	float distance_01 = framebuffer_depth - particle_depth;

	distance_01 = clamp(distance_01,0.0,1.0);

              

	float cameraDistance_01 = particle_depth - u_FMinusN_FPlusN_FTimesNTimes2_N.w;             
	float soft_camera_start = 1.0;
	cameraDistance_01 -= soft_camera_start;
	cameraDistance_01 = clamp(cameraDistance_01,0.0,1.0);
	result.a *= distance_01 * cameraDistance_01;

                
	gl_FragColor = result;
}