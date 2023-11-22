#version 330

#define PI 3.1415
const float alpha_power = 2.0;

in vec2 texCoord;
in vec4 color;
out vec4 outColor;

uniform float u_time;
uniform float u_move;

void main(void) {

	vec2 uv = texCoord * 2.0 - 1.0;

	float div = 0.5 * abs(sin(u_time + PI * u_move)) + 0.7;

    float distance = 1.0 - length(uv / div);

    /*if (distance < 0.5) {
        color.r = 0.85;
        color.g = 0.4;
    }*/

    if (distance < 0.0)
       discard;


	outColor = vec4(color.rgb, pow(distance, alpha_power) * color.a);
}