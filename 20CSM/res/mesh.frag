#version 410 core

precision highp float;

in vec2 v_texccord;
in vec3 v_normal;
in vec4 v_fragPos;

uniform vec4 color = vec4(1.0, 1.0, 1.0, 1.0);
uniform vec3 lightPos = vec3(0.0, 0.0, 0.0);
uniform vec3 u_viewPos = vec3(0.0, 0.0, -1.0);
uniform sampler2D u_texture;

vec4 ambMat = vec4(0.2, 0.2, 0.2, 1.0);
vec4 ambLight = vec4(0.0, 0.0, 0.0, 1.0);

vec4 diffMat = vec4(0.8, 0.8, 0.8, 1.0);
vec4 diffLight = vec4(1.0);

vec4 specMat = vec4(0.0, 0.0, 0.0, 1.0);
vec4 specLight = vec4(1.0);

out vec4 outColor;

void main(void) {
	vec3 N = normalize(v_normal);
	vec3 L = normalize(lightPos - v_fragPos.xyz);
	vec3 V = normalize(u_viewPos - v_fragPos.xyz);
	vec3 H = normalize(L + V);

	//glDisable(GL_COLOR_MATERIAL);
	vec4 ambient = ambMat * ambLight;
	vec4 diffuse = diffMat * diffLight * max(dot(L, N), 0.0);
	vec4 specular = specMat * specLight * pow(max(dot(H, N), 0.0), 0.0);
	
	//glEnable(GL_COLOR_MATERIAL);
	//vec3 ambient = color.xyz * ambLight.xyz;
	//vec3 diffuse = color.xyz * diffLight.xyz * max(dot(L, N), 0.0);
	//vec3 specular = specMat.xyz * specLight.xyz * pow(max(dot(H, N), 0.0), 0.0);
	
	//flip here cuz soli2 don't support flipping on rawDDs
	//outColor = texture2D(u_texture, vec2(v_texccord.x, 1.0 - v_texccord.y));
	//outColor = color;
	outColor = ambient + diffuse + specular;
	//outColor = vec4(N, 1.0);
}