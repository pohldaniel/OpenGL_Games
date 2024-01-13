#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);

out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_position;

void main(void){
   gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
   v_texCoord = i_texCoord;
   v_normal = mat3(u_normal) * i_normal;
   v_position = (u_model * vec4(i_position, 1.0)).xyz;
}