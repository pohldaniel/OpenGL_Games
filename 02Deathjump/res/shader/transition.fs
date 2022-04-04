#version 330

float diamondPixelSize = 50.f;

in vec2 texCoord;
out vec4 outColor;

uniform sampler2D u_texture;
uniform float u_progress;

void main(void) {
	vec2 uv = texCoord * (vec2(1.0, 1.0) - texCoord);

    float vig = uv.x * uv.y * 50.0;

    vig = pow(vig, 0.25); 

    vec4 pixel = texture(u_texture, texCoord);

    float UVx = texCoord.x;
    float UVy = texCoord.y;

    float xFraction = fract(gl_FragCoord.x / diamondPixelSize);
    float yFraction = fract(gl_FragCoord.y / diamondPixelSize);

    float xDistance = abs(xFraction - 0.5);
    float yDistance = abs(yFraction - 0.5);

	if (xDistance + yDistance + UVx + UVy > u_progress * 4.f) {
        discard;
    }

	outColor = vig * pixel;
}
