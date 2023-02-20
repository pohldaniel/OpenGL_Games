#version 410 core

in vec3 v_Normal;
in vec4 v_fragPos;
out vec4 fragColor;

uniform vec3 color = vec3(1.0, 0.0, 0.0);
uniform sampler2D tex;
uniform vec3 lightPos = vec3(0.0, 0.0, -1.0);
uniform vec3 u_viewPos = vec3(0.0, 0.0, -1.0);

void main() {

	vec3 N = normalize(v_Normal);
	vec3 L = normalize(lightPos - v_fragPos.xyz);
	vec3 V = normalize(u_viewPos - v_fragPos.xyz);
	vec3 H = normalize(L + V);
	
	vec3 ambient = color * 0.1;
	vec3 diffuse = color * 0.9 * max(dot(L, N), 0.0);
	vec3 specular = vec3(1.0, 1.0, 1.0) * pow(max(dot(H, N), 0.0), 16.0);

	fragColor = vec4(ambient + diffuse + specular, 1.0);
}