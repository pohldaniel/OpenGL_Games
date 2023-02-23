#version 410 core

in vec2 texCoord;

uniform vec2 SSTexelSize = vec2(0.5, 0.5);
uniform float blendFactor = 1.0;
uniform sampler2D u_texture;

out vec4 color;

void main(){

	vec4 tap0 = texture(u_texture, texCoord);
    vec4 tap1 = texture(u_texture, texCoord + SSTexelSize * vec2(  0.4,  0.9 ));
    vec4 tap2 = texture(u_texture, texCoord + SSTexelSize * vec2( -0.4, -0.9 ));
    vec4 tap3 = texture(u_texture, texCoord + SSTexelSize * vec2( -0.9,  0.4 ));
    vec4 tap4 = texture(u_texture, texCoord + SSTexelSize * vec2(  0.9, -0.4 ));
    color = 0.2 * ( tap0 + tap1 + tap2 + tap3 + tap4 );
}