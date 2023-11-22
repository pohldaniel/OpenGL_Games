#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 2) in vec3 i_normal;

out vec3 vNormal;
out vec3 vPosition;

uniform mat4 Projection;
uniform mat4 Modelview;
uniform mat4 NormalMatrix;

void main()
{
    vPosition = (Modelview * vec4(i_position, 1.0)).xyz;
    vNormal = mat3(NormalMatrix) * i_normal;
    gl_Position = Projection * Modelview * vec4(i_position, 1.0);
}
