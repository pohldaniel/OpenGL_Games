#version 430 core

layout (local_size_x =16, local_size_y = 16) in;
layout(binding=0, rgba8) uniform mediump image2D inputImage;
layout(binding=1, rgba8) uniform mediump image2D resultImage;

void main()
{
        float u = float(gl_GlobalInvocationID.x);
        float v = float(gl_GlobalInvocationID.y);
        
        vec4 inv = 1.0 - imageLoad(inputImage, ivec2(u,v));
        imageStore(resultImage, ivec2(u,v), inv);

}

