#version 410 core

in vec2 texCoord;

uniform vec2 SSTexelSize;
uniform float blendFactor = 1.0;
uniform sampler2D u_texture;

out vec4 color;

void main(){

	vec4 tap0 = texture(u_texture, texCoord);
    vec4 tap1 = texture(u_texture, texCoord + SSTexelSize * vec2(  0.4,  0.9 ));
    vec4 tap2 = texture(u_texture, texCoord + SSTexelSize * vec2( -0.4, -0.9 ));
    vec4 tap3 = texture(u_texture, texCoord + SSTexelSize * vec2( -0.9,  0.4 ));
    vec4 tap4 = texture(u_texture, texCoord + SSTexelSize * vec2(  0.9, -0.4 ));
    vec4 color1 = 0.2 * ( tap0 + tap1 + tap2 + tap3 + tap4 );
	
	vec4 tap11 = texture(u_texture, texCoord + SSTexelSize * vec2(  0.9,  1.9 ));
    vec4 tap21 = texture(u_texture, texCoord + SSTexelSize * vec2( -0.9, -1.9 ));
    vec4 tap31 = texture(u_texture, texCoord + SSTexelSize * vec2( -1.9,  0.9 ));
    vec4 tap41 = texture(u_texture, texCoord + SSTexelSize * vec2(  1.9, -0.9 ));
    vec4 color2 = 0.2 * ( tap0 + tap11 + tap21 + tap31 + tap41 );

    float mask = clamp(color2.w, 0.0, 1.0);

    color = mix(color2, color1, mask);
    color.w = mask;
}