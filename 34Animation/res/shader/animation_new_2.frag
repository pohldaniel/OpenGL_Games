#version 430 core

in vec2 v_texCoord;
in vec3 v_normal;
in vec4 vertColor;

uniform sampler2D u_texture;
uniform vec3 u_light = vec3(1.0, 1.0, 1.0);

out vec4 outColor;

void main() {


	outColor = vertColor * texture2D(u_texture, v_texCoord);
}