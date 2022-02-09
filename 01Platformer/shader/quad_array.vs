#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 fTexCoord;

uniform mat4 transform;

void main(void) {

	gl_Position = transform * vec4(i_position, 1.0);  
    fTexCoord  = i_texCoord;
}
