#version 430 core
#extension GL_ARB_bindless_texture : require

#define NUM_SDFS 16

uniform sampler3D u_texture;
uniform uint u_layer = 0;

layout(std140, binding = 2) uniform SDFTextures {
    sampler3D sdf[NUM_SDFS];
};

in vec3 texCoord;

out vec4 color;

void main(void){
	color = texture( sdf[u_layer], texCoord );	
}
