//--------------------------------------------------------------------------------------
// Order Independent Transparency with Dual Depth Peeling
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#version 410 core

layout(location=0) out vec4 outColor;

uniform sampler2DRect TempTex;

void main(void)
{
	outColor = texture2DRect(TempTex, gl_FragCoord.xy);
	// for occlusion query
	if (outColor.a == 0) discard;
}
