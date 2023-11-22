#version 430 core

out vec4 FragColor;
in vec3 vPosition;

uniform sampler2D RayStart;
uniform sampler2D RayStop;
uniform sampler3D Volume;
uniform sampler2D Noise;

uniform float StepLength = 0.01;
uniform float Threshold = 0.45;

uniform vec3 LightPosition;
uniform vec3 DiffuseMaterial;
uniform mat4 NormalMatrix;

float lookup(vec3 coord)
{
    vec3 V = texture(Volume, coord).xyz;
    return dot(V, V);
}

void main()
{
    vec2 coord = 0.5 * (vPosition.xy + 1.0);
    vec3 rayStart = texture(RayStart, coord).xyz;
    vec3 rayStop = texture(RayStop, coord).xyz;

    if (rayStart == rayStop) {
        discard;
        return;
    }

    vec3 ray = rayStop - rayStart;
    float rayLength = length(ray);
    vec3 stepVector = StepLength * ray / rayLength;

    // Jitter Z to hide "wood grain" artifacts:
    rayStart += stepVector * texture(Noise, gl_FragCoord.xy / 256).r;

    vec3 pos = rayStart;
    vec4 dst = vec4(0);
    while (dst.a < 1 && rayLength > 0) {
        
        float V = lookup(pos);
        if (V > Threshold) {

            // Refine the hitpoint to reduce slicing artifacts:
            vec3 s = -stepVector * 0.5;
            pos += s; V = lookup(pos);
            if (V > Threshold) s *= 0.5; else s *= -0.5;
            pos += s; V = lookup(pos);

            if (V > Threshold) {
                float L = StepLength;
                float E = lookup(pos + vec3(L,0,0));
                float N = lookup(pos + vec3(0,L,0));
                float U = lookup(pos + vec3(0,0,L));
                vec3 normal = normalize(mat3(NormalMatrix) * vec3(E - V, N - V, U - V));
                vec3 light = LightPosition;

                float df = abs(dot(normal, light));
                vec3 color = df * DiffuseMaterial;

                vec4 src = vec4(color, 1.0);
                dst = (1.0 - dst.a) * src + dst;
                break;
            }
        }

        pos += stepVector;
        rayLength -= StepLength;
    }

    FragColor = dst;
}