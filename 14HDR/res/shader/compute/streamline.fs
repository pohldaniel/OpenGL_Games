#version 430 core

out vec4 FragColor;
in float gAlpha;
uniform float Brightness = 0.5;

void main(){
    FragColor = Brightness * vec4(gAlpha, 0, 0, 1);
}