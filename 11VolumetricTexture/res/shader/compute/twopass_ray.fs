#version 430 core

out vec4 FragColor;
in vec2 gTexCoord;

uniform sampler2D RayStartPoints;
uniform sampler2D RayStopPoints;
uniform sampler3D Density;

uniform vec3 LightPosition = vec3(0.25,1.0,3);
uniform vec3 LightIntensity = vec3(15);
uniform float Absorption = 1.0;
uniform mat4 Modelview;
uniform mat4 ViewMatrix;

const float maxDist = sqrt(2.0);
const int numSamples = 128;
const float stepSize = maxDist/float(numSamples);
const int numLightSamples = 32;
const float lscale = maxDist / float(numLightSamples);
const float densityFactor = 5;

void main()
{
    vec3 rayStart = texture(RayStartPoints, gTexCoord).xyz;
    vec3 rayStop = texture(RayStopPoints, gTexCoord).xyz;

    if (rayStart == rayStop)
    {
        FragColor = vec4(1);
        return;
    }

    rayStart = 0.5 * (rayStart + 1.0);
    rayStop = 0.5 * (rayStop + 1.0);

    vec3 pos = rayStart;
    vec3 step = normalize(rayStop-rayStart) * stepSize;
    float travel = distance(rayStop, rayStart);
    float T = 1.0;
    vec3 Lo = vec3(0.0);

    for (int i=0; i < numSamples && travel > 0.0; ++i, pos += step, travel -= stepSize) {

        float density = texture(Density, pos).x * densityFactor;
        if (density <= 0.0) {
            continue;
        }
        T *= 1.0-density*stepSize*Absorption;
        if (T <= 0.01)
            break;

        vec3 lightDir = normalize(LightPosition-pos)*lscale;
        float Tl = 1.0;
        vec3 lpos = pos + lightDir;

        for (int s=0; s < numLightSamples; ++s) {
            float ld = texture(Density, lpos).x;
            Tl *= 1.0-Absorption*lscale*ld;
            if (Tl <= 0.01) 
            lpos += lightDir;
        }

        vec3 Li = LightIntensity*Tl;
        Lo += Li*T*density*stepSize;
    }

    FragColor.rgb = Lo;
    FragColor.a = 1-T;
}
