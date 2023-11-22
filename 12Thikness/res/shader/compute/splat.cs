#version 430 core

layout (local_size_x =8, local_size_y = 8, local_size_z = 8) in;
layout(binding=0, rgba16f) restrict writeonly uniform image3D resultImage;

uniform vec3 centre;
uniform float radius;
uniform vec4 color;

void main(){
	
	ivec3 dims = imageSize(resultImage);
    ivec3 pixel_coords = ivec3(gl_GlobalInvocationID.xyz);
	
	float u = float(pixel_coords.x);
	float v = float(pixel_coords.y);
	float w = float(pixel_coords.z);
	
    vec3 x = vec3(u / dims.x, v/ dims.y, w/ dims.z);
	
	float d = length(x - centre);
    vec4 color = (1.0 - smoothstep(0.0, radius, d)) * color;
    imageStore(resultImage, pixel_coords, color);
}