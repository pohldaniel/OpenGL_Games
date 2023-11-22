#version 430 core

layout (local_size_x =8, local_size_y = 8, local_size_z = 8) in;
layout(binding=0, rgba16f) restrict readonly uniform image3D currentTex;
layout(binding=1, rgba16f) restrict readonly uniform image3D prevTex;
layout(binding=2, rgba16f) restrict writeonly uniform image3D resultImage;

uniform vec3 voxelSize;
uniform float damping;


void main(){
	
	ivec3 dims = imageSize(resultImage);
    ivec3 x = ivec3(gl_GlobalInvocationID.xyz);
	
	vec4 smoothed;
    smoothed = imageLoad(currentTex, x + ivec3(1, 0, 0));
	smoothed += imageLoad(currentTex, x + ivec3(-1, 0, 0));
	smoothed += imageLoad(currentTex, x + ivec3(0, 1, 0));
	smoothed += imageLoad(currentTex, x + ivec3(0, -1, 0));
	smoothed += imageLoad(currentTex, x + ivec3(0, 0, 1));
	smoothed += imageLoad(currentTex, x + ivec3(0, 0, -1));
	smoothed /= 6.0;
	
	vec4 prev = imageLoad(prevTex, x);
	
    vec4 pixel = (smoothed * 2.0 - prev) * damping;
    imageStore(resultImage, x, pixel);
	
	//imageStore(resultImage, x, imageLoad(currentTex, x));
}