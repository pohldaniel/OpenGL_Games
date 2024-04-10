#version 410 core

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_view;
uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normalMatrix;

uniform mat4 u_projectionShadow[4];
uniform mat4 u_viewShadow[4];
uniform mat4 u_projectionShadowBias[4];

uniform mat4 u_currWorldViewProj;
uniform mat4 u_prevWorldViewProj;
uniform vec4 u_color = vec4(1.0);
uniform vec3 u_cameraPosition;
uniform vec2 u_jitter = vec2(0.0, 0.0);

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;


out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec2 v_texCoord;
out vec3 v_worldPosition;
out vec3 v_view;
out vec4 sv_position;
out vec4 v_currPosition;
out vec4 v_prevPosition;
out vec4 vertColor;

out vec4 sc[4];


void main(){    

	v_normal  = (u_normalMatrix * vec4(i_normal, 0.0)).xyz;
	v_tangent = (u_normalMatrix * vec4(i_tangent, 0.0)).xyz;
	v_bitangent = (u_normalMatrix * vec4(i_bitangent, 0.0)).xyz;
	v_texCoord = i_texCoord;
	vertColor = u_color;
	
	v_worldPosition = (u_model * vec4(i_position, 1.0)).xyz;
	v_view = u_cameraPosition - v_worldPosition;

    sv_position = u_projection * u_view * u_model * vec4(i_position, 1.0);
	gl_Position = sv_position;
	
	v_currPosition = sv_position;
	v_prevPosition = u_prevWorldViewProj * vec4(i_position, 1.0);
		
	//sv_position.xy -= u_jitter * sv_position.w;
	v_currPosition.xy = v_currPosition.xy * 0.5 + vec2(0.5) * v_currPosition.w;
	v_prevPosition.xy = v_prevPosition.xy * 0.5 + vec2(0.5) * v_prevPosition.w;
	
	
	vec3 shrinkedPos = i_position - 0.05 * normalize(i_normal);
	
	for(int i = 0; i < 4; i++){
		sc[i] = u_projectionShadow[i]   * u_viewShadow[i] * u_model * vec4(shrinkedPos, 1.0);
		sc[i].xyz = sc[i].xyz * 0.5 + vec3(0.5) * sc[i].w;
	}
}
