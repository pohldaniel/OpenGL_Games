#version 430 core

uniform float depthScale;

out float fragmentdepth;

void main() {
    fragmentdepth = depthScale * gl_FragCoord.z;
}