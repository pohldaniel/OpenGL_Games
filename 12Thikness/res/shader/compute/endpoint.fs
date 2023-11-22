#version 430 core

in vec3 vPosition;
out vec3 FragData[2];

void main()
{
    vec3 pos = vPosition;

    if (gl_FrontFacing) {
        FragData[0] = 0.5 * (pos + 1.0);
        FragData[1] = vec3(0);
    } else {
        FragData[0] = vec3(0);
        FragData[1] = 0.5 * (pos + 1.0);
    }
}