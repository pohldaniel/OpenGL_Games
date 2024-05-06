#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_vp = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform vec4 u_color = vec4(1.0); 

out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_tangent;
out vec3 v_bitangent;
out vec4 vertColor; 

void main(void){
   //gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
   
   //gl_Position = vec4(i_position, 1.0) * u_model * u_vp;
   gl_Position = u_vp * u_model * vec4(i_position, 1.0);
   
   v_texCoord = i_texCoord;
   v_normal = i_normal;
   v_tangent = i_tangent;
   v_bitangent = i_bitangent;
   vertColor = u_color;
}