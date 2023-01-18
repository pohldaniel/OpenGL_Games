#version 330

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D s_Albedo;
uniform sampler2D s_Normal;


const float kAmbient = 0.1;

void main(void) {
    vec3 dir = normalize(vec3(0.0, 1.0, 1.0));

    vec3 albedo = texture(s_Albedo, texCoord).rgb;
    vec3 normal = texture(s_Normal, texCoord).rgb;

    vec3 color = albedo * max(dot(normal, dir), 0.0) + albedo * kAmbient;

    outColor = vec4(color, 1.0);
}
