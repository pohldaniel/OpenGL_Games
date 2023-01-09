#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model = mat4(1.0);

out vec2 v_texCoord;
out vec3 v_worldPos;
out vec3 v_normal;

void main(void){
  
	vec4 worldPos = u_model * vec4(i_position, 1.0f);
    v_worldPos = worldPos.xyz;
    v_normal   = normalize(mat3(u_model) * i_normal);
    v_texCoord = i_texCoord;

    gl_Position = u_projection * u_view * worldPos;
}