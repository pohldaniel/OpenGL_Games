//--------------------------------------------------------------------------------------
// Order Independent Transparency with Weighted Sums
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#version 410 core

layout(location=0) out vec4 outColor;

uniform sampler2DRect ColorTex;
uniform vec3 BackgroundColor;

// Sum(A_i * C_i) + C_bg * (1 - Sum(A_i))
void main(void){
	vec4 S = texture2DRect(ColorTex, gl_FragCoord.xy);
	outColor.rgb = S.rgb + BackgroundColor * (1.0 - S.a);
}
