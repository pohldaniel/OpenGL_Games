#version 430 core

out vec4 FragColor;
in vec3 gObj;

void main()
{
    FragColor = vec4(gObj, 1);
}