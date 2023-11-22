#version 430 core


uniform mat4 NormalMatrix;

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;
in vec3 vPosition[3];
out vec3 gFacetNormal;
out vec3 gTriDistance;

void main()
{
    vec3 A = vPosition[2] - vPosition[0];
    vec3 B = vPosition[1] - vPosition[0];
    gFacetNormal = mat3(NormalMatrix) * normalize(cross(A, B));

    gTriDistance = vec3(1, 0, 0);
    gl_Position = gl_in[0].gl_Position; EmitVertex();

    gTriDistance = vec3(0, 1, 0);
    gl_Position = gl_in[1].gl_Position; EmitVertex();

    gTriDistance = vec3(0, 1, 1);
    gl_Position = gl_in[2].gl_Position; EmitVertex();

    EndPrimitive();
}