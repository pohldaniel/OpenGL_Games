#version 410 core

flat in uint v_layer;
in vec2 v_texCoord;
in vec4 v_color;									

out vec4 outColor;

uniform sampler2DArray u_texture;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

void main() {
    
	vec3 distances = texture(u_texture, vec3(v_texCoord, v_layer)).rgb;
	ivec2 sz = textureSize(u_texture, 0).xy;
	float dx = dFdx(v_texCoord.x) * sz.x; 
    float dy = dFdy(v_texCoord.y) * sz.y;
	float toPixels = 8.0 * inversesqrt(dx * dx + dy * dy);
    float sigDist = median(distances.r, distances.g, distances.b);
	float w = fwidth(sigDist);
    float opacity = smoothstep(0.5 - w, 0.5 + w, sigDist);
	
	outColor = vec4(v_color.rgb, opacity);
} 