#version 410 core

precision highp float;

in vec3 v_normal;
in vec4 v_fragPos;

uniform vec4 color = vec4(1.0);
uniform vec3 lightPos = vec3(0.0, 0.0, 1.0);
uniform vec3 u_viewPos = vec3(0.0, 0.0, -1.0);

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
	//vec4 ambient = ambMat * ambLight;
	//vec4 diffuse = diffMat * diffLight * max(dot(L, N), 0.0);
	//vec4 specular = specMat * specLight * pow(max(dot(H, N), 0.0), 0.0);
	
	//glEnable(GL_COLOR_MATERIAL);
	vec4 ambient = color * ambLight;
	vec4 diffuse = color * diffLight * max(dot(L, N), 0.0);
	vec4 specular = specMat * specLight * pow(max(dot(H, N), 0.0), 0.0);

	outColor = ambient + diffuse + specular;
}
//https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glMaterial.xml
//https://registry.khronos.org/OpenGL-Refpages/gl2.1/xhtml/glLight.xml