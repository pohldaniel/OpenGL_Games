#version 430 core
#extension GL_ARB_bindless_texture : require

#define NUM_INSTANCES 16
#define NUM_SDFS 16
#define INFINITY 100000.0f

uniform sampler3D u_texture;

layout(std140, binding = 2) uniform SDFTextures
{
    sampler3D sdf[NUM_SDFS];
};

in vec3 texCoord;

out vec4 color;

void main(void){
	color = texture( sdf[1], texCoord );	
	
	//color = vec4(texCoord, 1.0);
}
