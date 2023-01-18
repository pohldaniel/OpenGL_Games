#version 430 core

layout (local_size_x =8, local_size_y = 8, local_size_z = 8) in;
layout(binding=0, rgba16f) restrict readonly uniform image3D source;
layout(binding=1, rgba16f) restrict writeonly uniform image3D resultImage;

void main(){
    ivec3 dims = imageSize(resultImage);
    ivec3 x = ivec3(gl_GlobalInvocationID.xyz);

	imageStore(resultImage, x, imageLoad(source, x));
}