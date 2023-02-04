#version 410 core

in vec2 texCoord[4];

uniform sampler2D u_texture;

out vec4 color;

void main(){
    color = (texture(u_texture, texCoord[0]) + texture(u_texture, texCoord[1]) + texture(u_texture, texCoord[2]) + texture(u_texture, texCoord[3])) * 0.25;  
}