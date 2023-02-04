//--------------------------------------------------------------------------------------
// Order Independent Transparency with Average Color
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#version 410 core

layout(location=0) in vec3 inVertexPosition;
layout(location=2) in vec3 inNormal;
layout(location=0) out vec3 vTexCoord;

uniform mat4 uModelViewMatrix;
uniform mat4 uNormalMatrix;

vec3 ShadeVertex()
{
    float diffuse = abs(normalize(uNormalMatrix * vec4(inNormal, 0.0)).z);
    return vec3(inVertexPosition.xy, diffuse);
}

void main(void)
{
     gl_Position = uModelViewMatrix * vec4(inVertexPosition, 1.0);
	 vTexCoord = ShadeVertex();
}