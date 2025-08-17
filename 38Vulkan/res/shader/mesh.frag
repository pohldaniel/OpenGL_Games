#version 450
#extension GL_EXT_nonuniform_qualifier : enable

layout(location = 0) in vec2 fragUV;
layout(location = 0) out vec4 fragColor;

layout(set = 1, binding = 0) uniform sampler2D diffuse;

void main(){
    fragColor = vec4(texture(diffuse, fragUV).rgb, 1.0);
}
