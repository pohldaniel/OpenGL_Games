#version 430 core

layout(location = 0) out vec4 colorOut;
layout(location = 1) out float depthOut;

uniform sampler2D u_texture;

in vec2 vertTexCoord;
in vec3 vertNormal;
in vec4 vertColor;

uniform float u_near;
uniform float u_far;

float linearizeDepth(float depth){
    float z = depth * 2.0 - 1.0; // back to NDC
    return (2.0 * u_near * u_far) / (u_far + u_near - z * (u_far - u_near));
}

void main(void){
	colorOut = vertColor * texture2D( u_texture, vertTexCoord );
	colorOut.a = 1.0;
	
	depthOut = linearizeDepth(gl_FragCoord.z);
}
