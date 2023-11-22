#version 430 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in int vInstance[3];
in vec2 vPosition[3];
out vec3 gPosition;

uniform float InverseSize;

void main()
{
    gPosition.z = 1.0 - 2.0 * vInstance[0] * InverseSize;
    gl_Layer = int(vInstance[0]);

    gPosition.xy = vPosition[0];
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    gPosition.xy = vPosition[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    gPosition.xy = vPosition[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}