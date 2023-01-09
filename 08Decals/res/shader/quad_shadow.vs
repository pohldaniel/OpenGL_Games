#version 330

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;

out vec2 texCoord;
out vec4 sc;

uniform mat4 u_transform = mat4(1.0);

void main(void) {
	vec4 position = u_transform * vec4(i_position, 1.0);

    gl_Position = position;
    texCoord  = i_texCoord;

	sc = vec4(i_position, 1.0);
    sc.xyz = sc.xyz * 0.5 + vec3(0.5) * sc.w;
}
