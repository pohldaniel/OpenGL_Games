#version 430 core

layout(location = 0) out float distanceOut;

in float vertDistance;

float getDepth(float depth, float near, float far){
	float z_ndc = depth * 2.0 - 1.0;
	return (2.0 * near * far) / (far + near + z_ndc * (near - far));	//[near, far]
}

void main(void){
	distanceOut = vertDistance;
	//distanceOut = getDepth(gl_FragCoord.z, 1.0, 1024.0);
}
