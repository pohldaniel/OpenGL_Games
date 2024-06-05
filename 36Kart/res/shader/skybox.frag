#version 440 core

out vec4 color;
in vec3 texCoord;

uniform samplerCube u_texture;

void main() {
    color = texture(u_texture, texCoord);

} 