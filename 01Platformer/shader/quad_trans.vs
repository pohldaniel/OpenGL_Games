#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

flat out float alpha;
out vec2 texCoord;

uniform mat4 u_transform;
uniform mat4 u_frame;
uniform float u_alpha = 1.0f;

void main(void) {
	gl_Position = u_transform * vec4(i_position, 1.0);  
	texCoord  = (u_frame * vec4(i_texCoord, 0.0, 1.0)).xy;     
	alpha = u_alpha;
}
