#version 430 core

in vec4 Position;
out vec2 vPosition;
out int vInstance;
uniform vec4 Center;

void main()
{
    gl_Position = Position + Center;
    vPosition = Position.xy;
    vInstance = gl_InstanceID;
}