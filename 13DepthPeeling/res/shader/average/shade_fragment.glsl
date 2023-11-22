//--------------------------------------------------------------------------------------
// Order Independent Transparency Fragment Shader
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#version 410 core

layout(location=0) in vec3 vTexCoord;

uniform float Alpha;

#define COLOR_FREQ 30.0
#define ALPHA_FREQ 30.0

#if 1
vec4 ShadeFragment()
{
	vec4 color;

    float xWorldPos = vTexCoord.x;
    float yWorldPos = vTexCoord.y;
    float diffuse = vTexCoord.z;

    float Freq = 4.0;
    float i = floor(xWorldPos * Freq);
    float j = floor(yWorldPos * Freq);
    color.rgb = (mod(i, 2.0) == 0) ? vec3(.4,.85,.0) : vec3(1.0);

    color.rgb *= diffuse;
    color.a = Alpha;
    return color;

	
}
#else
vec4 ShadeFragment()
{
	float xWorldPos = vTexCoord.x;
	float yWorldPos = vTexCoord.y;
	float diffuse = vTexCoord.z;

	vec4 color;
	float i = floor(xWorldPos * COLOR_FREQ);
	float j = floor(yWorldPos * ALPHA_FREQ);
	color.rgb = (mod(i, 2.0) == 0) ? vec3(.4,.85,.0) : vec3(1.0);
	//color.a = (mod(j, 2.0) == 0) ? Alpha : 0.2;
	color.a = Alpha;

	color.rgb *= diffuse;
	return color;
}
#endif
