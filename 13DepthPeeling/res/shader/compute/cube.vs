#version 430 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 texCoords;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_modelView= mat4(1.0);

out vec3 v_texCoord;

void main(void) {
   gl_Position = u_projection * u_modelView  * vec4(position, 1.0);
   v_texCoord = (position + vec3(1.0, 1.0, 1.0)) * 0.5;
}
