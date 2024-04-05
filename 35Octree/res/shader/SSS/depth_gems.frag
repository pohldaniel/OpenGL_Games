#version 430 core

in float distance;

out vec4 outColor;

void main() {
  outColor = vec4(distance);
}
