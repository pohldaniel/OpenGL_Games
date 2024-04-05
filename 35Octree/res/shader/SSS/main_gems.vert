#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;

uniform mat4 u_projection = mat4(1.0);
uniform mat4 u_view = mat4(1.0);
uniform mat4 u_model = mat4(1.0);
uniform mat4 u_normal = mat4(1.0);

struct DirectionalLightData{
    mat4 biasProjView;
    mat4 projView;
    mat4 view;
    vec3 dir;
};

uniform DirectionalLightData light;

out vec3 vertexNormal;
out vec3 lightDir;
out vec3 vertexCameraNormal;
out vec3 camDir;
out vec4 vertexLightCoord;
out float vertexLightDist;

void main() {
  gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0);
  camDir = -vec3(u_view *vec4(i_position, 1.0));
  vertexNormal = i_normal;
  vertexCameraNormal = normalize(mat3(u_normal)* i_normal);
  
  vertexLightCoord = light.biasProjView * u_model * vec4(i_position, 1.0);
  vertexLightDist = length(light.view * u_model * vec4(i_position, 1.0));
  lightDir = -light.dir;
}
