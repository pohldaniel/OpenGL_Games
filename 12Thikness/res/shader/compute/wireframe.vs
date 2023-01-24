#version 430 core

in vec4 Position;
out vec3 vPosition;
uniform mat4 ModelviewProjection;

void main()
{
    gl_Position = ModelviewProjection * Position;
    vPosition = Position.xyz;
}