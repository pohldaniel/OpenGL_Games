 #version 430 core

uniform mat4 mvp;

layout (location = 0) in vec3 position;

void main(void)
{
    gl_Position = mvp * vec4(position, 1.0);
}
