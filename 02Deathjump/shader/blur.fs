#version 330

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D u_texture;
uniform float u_blur_radius;

void main(void) {

	vec2 offx = vec2(u_blur_radius, 0.0);
    vec2 offy = vec2(0.0, u_blur_radius);

    vec4 pixel = texture2D(u_texture, texCoord) * 4.0 +
        texture2D(u_texture, texCoord.xy - offx) * 2.0 +
        texture2D(u_texture, texCoord.xy + offx) * 2.0 +
        texture2D(u_texture, texCoord.xy - offy) * 2.0 +
        texture2D(u_texture, texCoord.xy + offy) * 2.0 +
        texture2D(u_texture, texCoord.xy - offx - offy) * 1.0 +
        texture2D(u_texture, texCoord.xy - offx + offy) * 1.0 +
        texture2D(u_texture, texCoord.xy + offx - offy) * 1.0 +
        texture2D(u_texture, texCoord.xy + offx + offy) * 1.0;

	outColor = (pixel / 16.0);
}
