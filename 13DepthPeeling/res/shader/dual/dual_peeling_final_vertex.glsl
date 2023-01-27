//--------------------------------------------------------------------------------------
// Order Independent Transparency with Dual Depth Peeling
//
// Author: Louis Bavoil
// Email: sdkfeedback@nvidia.com
//
// Copyright (c) NVIDIA Corporation. All rights reserved.
//--------------------------------------------------------------------------------------

#version 410 core

layout(location=0) in vec3 inVertexPosition;

uniform mat4 uModelViewMatrix;

void main(void) {
     gl_Position = uModelViewMatrix * vec4(inVertexPosition, 1.0);
}
