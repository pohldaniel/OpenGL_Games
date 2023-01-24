#version 430

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

out vec2 texCoord;
out vec4 sc;
out vec3 toCameraVector;
out vec3 fromLightVector;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);
uniform vec3 cameraPos;
uniform vec3 lightPosition;
uniform float tilingFactor = 10.0f;

void main(void) {
	vec4 position = u_projection * u_view * vec4(i_position, 1.0);

    gl_Position = position;
	texCoord = i_texCoord * tilingFactor;
	
	sc = position;
    sc.xyz = sc.xyz * 0.5 + vec3(0.5) * sc.w;
	
	toCameraVector = cameraPos - i_position;
	fromLightVector = i_position - lightPosition;
}