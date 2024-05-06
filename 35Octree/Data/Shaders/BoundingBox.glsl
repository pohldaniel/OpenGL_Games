#include "Uniforms.glsl"

#ifdef COMPILEVS

#include "Transform.glsl"

in vec3 position;

#else

out vec4 fragColor;

#endif

uniform mat4x4 viewProjMatrix2;


void vert()
{
    mat3x4 modelMatrix = GetWorldMatrix();
    vec3 worldPos = vec4(position, 1.0) * modelMatrix;
    gl_Position = vec4(worldPos, 1.0) * viewProjMatrix2;
}

void frag()
{
    fragColor = vec4(1.0, 1.0, 1.0, 0.5);
}
