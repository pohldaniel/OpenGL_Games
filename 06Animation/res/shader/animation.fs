#version 430 core

in vec2 v_texCoord;
in vec3 v_normal;

uniform sampler2D diffuse;

out vec4 color;

uniform vec3 u_light;  


out vec4 FragColor;

void main() {
	vec4 diffuseColor = texture2D(diffuse, v_texCoord);

	vec3 n = normalize(v_normal);
	vec3 l = normalize(u_light);
	float diffuseIntensity = clamp(dot(n, l) + 0.1, 0, 1);

	color = diffuseColor * diffuseIntensity;
	
	//color = vec4(n, 1.0);
}