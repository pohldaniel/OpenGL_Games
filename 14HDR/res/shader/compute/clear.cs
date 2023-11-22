#version 430 core

layout (local_size_x =8, local_size_y = 8, local_size_z = 8) in;
layout(binding=0, rgba16f) restrict writeonly uniform image3D resultImage;

uniform vec4 color = vec4(1.0, 0.0, 0.0, 0.0);

void main(){
	
    ivec3 pixel_coords = ivec3(gl_GlobalInvocationID.xyz);
    imageStore(resultImage, pixel_coords, color);
}