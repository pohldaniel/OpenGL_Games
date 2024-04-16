#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform vec4 u_color = vec4(1.0); 

uniform float instCnt = 1.0;
uniform float stackHeight = 0.0;
	
out vec3 fragNorm;
out vec3 fragLPos;
out vec3 fragWPos;
out vec3 fragVPos;
out vec2 fragUV;
out float instGrad;
out vec4 vertColor; 

void main(void){
	vec3 pos     = i_position;
	instGrad     = float( gl_InstanceID ) / instCnt;
	pos.y       += instGrad * stackHeight;


	vec4 wPos   = u_model * vec4( pos, 1.0 );
	vec4 vPos   = u_view * wPos;
	fragLPos    = i_position;
	fragWPos    = wPos.xyz;
	fragVPos    = vPos.xyz;
	fragNorm    = i_normal;
	fragUV      = i_texCoord;


	gl_Position = u_projection * vPos;
	vertColor = u_color;
}