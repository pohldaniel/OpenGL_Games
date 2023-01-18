#version 410 core

uniform sampler2D u_colorTex;
uniform sampler2D u_depthTex;
uniform sampler2D u_strengthTex;

uniform float sssWidth = 180.0;
uniform float id = 1.0;
uniform vec2 dir;
uniform bool initStencil = false;
      
in vec3 v_color;
in vec2 v_texCoord;

out vec4 colorBlurred;

const vec4 kernel2[25] = vec4[25] (
    vec4(0.530605, 0.613514, 0.739601, 0),
    vec4(0.000973794, 1.11862e-005, 9.43437e-007, -3),
    vec4(0.00333804, 7.85443e-005, 1.2945e-005, -2.52083),
    vec4(0.00500364, 0.00020094, 5.28848e-005, -2.08333),
    vec4(0.00700976, 0.00049366, 0.000151938, -1.6875),
    vec4(0.0094389, 0.00139119, 0.000416598, -1.33333),
    vec4(0.0128496, 0.00356329, 0.00132016, -1.02083),
    vec4(0.017924, 0.00711691, 0.00347194, -0.75),
    vec4(0.0263642, 0.0119715, 0.00684598, -0.520833),
    vec4(0.0410172, 0.0199899, 0.0118481, -0.333333),
    vec4(0.0493588, 0.0367726, 0.0219485, -0.1875),
    vec4(0.0402784, 0.0657244, 0.04631, -0.0833333),
    vec4(0.0211412, 0.0459286, 0.0378196, -0.0208333),
    vec4(0.0211412, 0.0459286, 0.0378196, 0.0208333),
    vec4(0.0402784, 0.0657244, 0.04631, 0.0833333),
    vec4(0.0493588, 0.0367726, 0.0219485, 0.1875),
    vec4(0.0410172, 0.0199899, 0.0118481, 0.333333),
    vec4(0.0263642, 0.0119715, 0.00684598, 0.520833),
    vec4(0.017924, 0.00711691, 0.00347194, 0.75),
    vec4(0.0128496, 0.00356329, 0.00132016, 1.02083),
    vec4(0.0094389, 0.00139119, 0.000416598, 1.33333),
    vec4(0.00700976, 0.00049366, 0.000151938, 1.6875),
    vec4(0.00500364, 0.00020094, 5.28848e-005, 2.08333),
    vec4(0.00333804, 7.85443e-005, 1.2945e-005, 2.52083),
    vec4(0.000973794, 1.11862e-005, 9.43437e-007, 3)
);

const vec4 kernel[17] = vec4[17](
	vec4(0.0727469,  0.0798656,  0.0871559,  0),
	vec4(0.0134888,  0.0110569,  0.00889932,  -3.64969),
	vec4(0.0336357,  0.0284734,  0.0238736,  -2.7943),
	vec4(0.0450509,  0.0401367,  0.0355976,  -2.05295),
	vec4(0.0594925,  0.0560719,  0.0523212,  -1.42566),	
	vec4(0.0734215,  0.0727698,  0.0715558,  -0.912423),
	vec4(0.082168,  0.0850056,  0.0872929,  -0.513238),
	vec4(0.0823918,  0.0872948,  0.0921819,  -0.228106),
	vec4(0.0739774,  0.079258,  0.0846997,  -0.0570264),
	vec4(0.0739774,  0.079258,  0.0846997,  0.0570264),	
	vec4(0.0823918,  0.0872948,  0.0921819,  0.228106),
	vec4(0.082168,  0.0850056, 0.087293,  0.513238),
	vec4(0.0734215,  0.0727698,  0.0715558,  0.912423),
	vec4(0.0594925,  0.0560719,  0.0523213,  1.42566),
	vec4(0.0450509,  0.0401367,  0.0355976,  2.05295),	
	vec4(0.0336357, 0.0284734,  0.0238736,  2.7943),
	vec4(0.0134888,  0.0110569,  0.00889932,  3.64969)
);

void main(void){

	vec4 colorM = texture2D( u_colorTex, v_texCoord);
	//vec4 colorM1 = texture2D( u_depthTex, v_texCoord);
	//vec4 colorM2 = texture2D( u_strengthTex, v_texCoord);
	
	float colorMa = colorM.a;
    // Initialize the stencil buffer in case it was not already available:
    if (initStencil) // (Checked in compile time, it's optimized away)
        if (colorMa == 0.0) discard;
		
		// Fetch linear depth of current pixel:
    float depthM = texture2D(u_depthTex, v_texCoord).r;

    // Calculate the sssWidth scale (1.0 for a unit plane sitting on the
    // projection window):
    float distanceToProjectionWindow = 1.0 / tan(0.5 * radians(20.0));
    float scale = distanceToProjectionWindow / depthM;
	
	// Calculate the final step to fetch the surrounding pixels:
    vec2 finalStep = scale * dir;
    finalStep *= 1.0; // Modulate it using the alpha channel.
    finalStep *= 1.0 / (2.0 * sssWidth); // sssWidth in mm / world space unit, divided by 2 as uv coords are from [0 1]
	
    // Accumulate the center sample:
    colorBlurred = colorM;
    colorBlurred.rgb *= kernel[0].xyz;
	
	 for (int i = 1; i < 17; i++) {
        // Fetch color and depth for current sample:
        vec2 offset = v_texCoord + kernel[i].w * finalStep;
        vec4 color = texture2D( u_colorTex, offset );
		
		 colorBlurred.rgb += kernel[i].xyz * color.rgb;
	}
}
