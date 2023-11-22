#version 410 core

in vec2 texCoord;

uniform sampler2D u_texture;

out vec4 color;

void main(){
    color = texture(u_texture, texCoord);   
}