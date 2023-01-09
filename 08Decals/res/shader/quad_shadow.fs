#version 330

in vec2 texCoord;
in vec4 sc;
out float outColor;

uniform sampler2D u_texture;

float getDepthPassSpaceZ(float zWC, float near, float far){

	// Assume standard opengl depth range [0..1]
    float z_n = 2.0 * zWC - 1.0;
    //float z_e = (2.0 * near * far) / (far + near + z_n * (near - far));	//[near, far]

	//divided by far to get the range [near/far, 1.0] just for visualisation
	float z_e = (2.0 * near) / (far + near + z_n * (near - far));	

	return z_e;
}

void main(void) {

	float depth = texture(u_texture, texCoord).r;
	outColor = getDepthPassSpaceZ(depth, 1.0, 5000.0);
}
