//--------------------------------------------------------------------------------------
// Order Independent Transparency with Average Color
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#version 410 core

layout(location=0) out vec4 outColor;

uniform sampler2DRect ColorTex0;
uniform sampler2DRect ColorTex1;
uniform vec3 BackgroundColor;

void main(void)
{
	vec4 SumColor = texture2DRect(ColorTex0, gl_FragCoord.xy);
	float n = texture2DRect(ColorTex1, gl_FragCoord.xy).r;

	if (n == 0.0) {
		outColor.rgb = BackgroundColor;
		return;
	}

	vec3 AvgColor = SumColor.rgb / SumColor.a;
	float AvgAlpha = SumColor.a / n;

	float T = pow(1.0-AvgAlpha, n);
	outColor.rgb = AvgColor * (1 - T) + BackgroundColor * T;
}
