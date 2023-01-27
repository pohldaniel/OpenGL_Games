#version 410 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoords;

uniform mat4 u_projection;
uniform mat4 u_modelView;
uniform mat4 u_invModelView;

out vec3 v_texCoord;
out vec3 v_eyerayo;
out vec3 v_eyerayd;

void main(void) {
   gl_Position =   u_projection * u_modelView  * vec4(position, 1.0);
   v_texCoord = texCoords;
   
   v_eyerayo = (u_invModelView * vec4(0, 0, 0, 1)).xyz;
   v_eyerayd = position - v_eyerayo;

}
