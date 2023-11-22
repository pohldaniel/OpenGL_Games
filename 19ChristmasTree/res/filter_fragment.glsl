#version 410 core

in vec2 texCoord;

uniform sampler2D tex;
uniform float sigma = 2.0;

//direction to perfom the gaussian
uniform vec2 direction = vec2( 1.0, 0.0);

//level to take the texture sample from
uniform float level = 0.0;

out vec4 outColor;

void main() {
	vec2 size = vec2( textureSize( tex, 0));
	vec2 offset = exp2( level) / size; 
	
	//compute the radius across the kernel
	int radius = int( floor(3.0*sigma) - 1.0);
	
	vec4 accum = vec4(0.0);
	
	//precompute factors used every iteration
	float sigmaSqx2 = sigma * sigma * 2.0;
	float sigmaxSqrt2PI = sqrt(2.0 * 3.1415926) * sigma;
	
	// separable Gaussian
    for ( int ii = -radius; ii <= radius; ii++) {
        float r = float(ii);
	    float factor = pow( 2.71828, -(r*r) / sigmaSqx2 ) / sigmaxSqrt2PI;
	    accum += factor * textureLod( tex, texCoord + r * direction * offset, level);
	}
		
	outColor = accum;
}

