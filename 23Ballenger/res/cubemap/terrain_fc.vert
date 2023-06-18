#version 410 core

layout (location = 0) in vec3 i_position;
layout (location = 1) in vec2 i_texCoords;
layout (location = 2) in vec3 i_normal;

out VertexData {
	vec3 normal;
	vec3 normalEye;
	vec3 pos;
	vec3 posEye;
	vec2 texCoord;
} outData;

out float planeMask;

uniform mat4 g_modelMatrix;
uniform mat4 g_viewMatrix;

int getVertexPlaneMask(vec3 v) { 
	return int(v.x < v.y) | 
		(int(v.x < -v.y) << 1) | 
		(int(v.x <  v.z) << 2) | 
		(int(v.x < -v.z) << 3) | 
		(int(v.z <  v.y) << 4) | 
		(int(v.z < -v.y) << 5) |
		(int(v.x <    1) << 8) |
		(int(v.x >   -1) << 9) |
		(int(v.y <    1) << 10) |
		(int(v.y >   -1) << 11) |
		(int(v.z <    1) << 12) |
		(int(v.z >   -1) << 13);
}


void main(void) {
    vec4 worldPos = g_modelMatrix * vec4(i_position, 1.0);
    vec4 viewPos = g_viewMatrix * worldPos;

	outData.pos = vec3(i_position);
	outData.posEye = vec3(viewPos);
	outData.texCoord = i_texCoords;
	outData.normal = i_normal;
	outData.normalEye = i_normal;
	
	viewPos.xyz /= viewPos.w;
	viewPos.w = 1;
    gl_Position = viewPos;

	planeMask = float(getVertexPlaneMask(worldPos.xyz));
}
