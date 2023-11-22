#version 430 core

in vec3 gPosition;
out vec3 FragColor;

uniform vec3  Color;
uniform float InverseVariance;
uniform float NormalizationConstant;

void main()
{
    float r2 = dot(gPosition, gPosition);
    FragColor = Color * NormalizationConstant * exp(r2 * InverseVariance);
}

