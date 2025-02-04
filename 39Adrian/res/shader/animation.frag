#version 430 core

in vec2 v_texCoord;
in vec3 v_normal;
in vec4 vertColor;

uniform sampler2D u_texture;
uniform vec3 u_light = vec3(-1.0, 1.0, -1.0);

out vec4 outColor;

void main() {


	vec3 n = normalize(v_normal);
	vec3 l = normalize(u_light);
	float diffuseIntensity = clamp(dot(n, l) + 0.1, 0, 1);

	outColor = vertColor * texture2D(u_texture, v_texCoord) * diffuseIntensity;
	outColor.a = 1.0;
}