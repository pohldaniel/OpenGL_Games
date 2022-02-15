#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 fTexCoord;

uniform mat4 u_transform;
uniform mat4 u_frame;

void main(void) {

	fTexCoord  = (u_frame * vec4(i_texCoord, 0.0, 1.0)).xy;
    gl_Position = u_transform * vec4(i_position, 1.0);   
}
