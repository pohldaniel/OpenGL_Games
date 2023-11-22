#version 330 core

layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 texCoord;

uniform mat4 u_transform = mat4(1.0);

void main(){
    gl_Position = u_transform * vec4(vertex.xy, 0.0, 1.0);
    texCoord = vertex.zw;
}