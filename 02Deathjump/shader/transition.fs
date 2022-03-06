#version 330

float diamondPixelSize = 50.f;

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D u_texture;

uniform vec2 u_resolution;
uniform float u_progress;

void main(void) {
	vec2 uv = gl_FragCoord.xy / u_resolution.xy;
	//vec2 uv = texCoord;

	uv *= 1.0 - uv.yx;

    float vig = uv.x * uv.y * 50.0;

    vig = pow(vig, 0.25); 

    vec4 pixel = texture(u_texture, texCoord);

    float UVx = gl_FragCoord.x / u_resolution.x;
    float UVy = gl_FragCoord.y / u_resolution.y;

    float xFraction = fract(gl_FragCoord.x / diamondPixelSize);
    float yFraction = fract(gl_FragCoord.y / diamondPixelSize);

    float xDistance = abs(xFraction - 0.5);
    float yDistance = abs(yFraction - 0.5);

	if (xDistance + yDistance + UVx + UVy > u_progress * 4.f) {
        discard;
    }

	outColor = pixel * vig;
}
