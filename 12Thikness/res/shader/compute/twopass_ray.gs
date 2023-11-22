#version 430 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

in vec4 vPosition[1];
out vec2 gTexCoord;

void main()
{
    gTexCoord = vec2(1, 0); gl_Position = vec4(+1, -1, 0, 1); EmitVertex();
    gTexCoord = vec2(1, 1); gl_Position = vec4(+1, +1, 0, 1); EmitVertex();
    gTexCoord = vec2(0, 0); gl_Position = vec4(-1, -1, 0, 1); EmitVertex();
    gTexCoord = vec2(0, 1); gl_Position = vec4(-1, +1, 0, 1); EmitVertex();
    EndPrimitive();
}