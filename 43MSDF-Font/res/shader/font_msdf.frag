#version 410 core

flat in uint v_layer;
in vec2 v_texCoord;
in vec4 v_color;									

out vec4 outColor;

uniform sampler2DArray u_texture;

//uniform float pxRange = 12.0; // set to distance field's pixel range

uniform float u_outline = 0.2;
uniform float u_in_bias = 0.0;
uniform float u_out_bias = 0.0;
uniform vec4 u_outlineColor = vec4(1.0, 0.0, 0.0, 1.0);
uniform float u_outline_width_relative = 0.2;
uniform float u_outline_width_absolute = 0.1;
uniform float u_threshold = 0.2;

uniform float u_rounded_fonts = 0.0;
uniform float u_rounded_outlines = 0.0;

const float imageSize = 1024.0;
const float distanceRange = 8.0;

float pxRange = 10.0;//3.0 is better for fullscreen; //: logrange(0.25, 256.0);
float thickness = 0.0; //: range(-1.0, +1.0);
float border = 0.125; //: range(0.0, 0.25);
vec2 shadowVector = vec2(+0.0625, -0.03125) * 0.1; //: range(-0.25, +0.25);
float shadowOpacity = 0.5; //: range(0.0, 1.0);

const vec3 bottomColor = vec3(0.0, 0.21875, 0.375);
const vec3 topColor = vec3(0.0, 0.625, 1.0);
const vec3 borderColor = vec3(1.0, 1.0, 1.0);
const vec4 shadowColor = vec4(1, 1, 0, 1);

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float getDistance() {
    vec3 msdf = texture(u_texture, vec3(v_texCoord, v_layer)).rgb;
    return median(msdf.r, msdf.g, msdf.b);
}


vec2 sqr(vec2 x) { return x*x; } // squares vector components

float screenPxRange() {
    vec2 unitRange = vec2(pxRange)/vec2(textureSize(u_texture, 0));
    // If inversesqrt is not available, use vec2(1.0)/sqrt
    vec2 screenTexSize = inversesqrt(sqr(dFdx(v_texCoord))+sqr(dFdy(v_texCoord)));
    // Can also be approximated as screenTexSize = vec2(1.0)/fwidth(v_texCoord);
    return max(0.5*dot(unitRange, screenTexSize), 1.0);
}

float remap(float x, vec2 from, vec2 to){ 
    return to.x + (x - from.x) / 
        (from.y - from.x) * (to.y - to.x);
}

float linearstep(float lo, float hi, float x){
	return (clamp(x, lo, hi) - lo) / (hi - lo);
}

void main() {
    
	/*vec3 distances = texture(u_texture, vec3(v_texCoord, v_layer)).rgb;
	ivec2 sz = textureSize(u_texture, 0).xy;
	float dx = dFdx(v_texCoord.x) * sz.x; 
    float dy = dFdy(v_texCoord.y) * sz.y;
	float toPixels = 8.0 * inversesqrt(dx * dx + dy * dy);
    float sigDist = median(distances.r, distances.g, distances.b);
	float w = fwidth(sigDist);
    float opacity = smoothstep(0.5 - w, 0.5 + w, sigDist);
	
	outColor = vec4(v_color.rgb, opacity);*/
	
	/*vec4 distances = texture(u_texture, vec3(v_texCoord, v_layer));
	float d_msdf = median(distances.r, distances.g, distances.b);
	float d_sdf = distances.a; // mtsdf format only
	float d_inner = mix(d_msdf, d_sdf, u_rounded_fonts);
	float d_outer = mix(d_msdf, d_sdf, u_rounded_outlines);
	float width = screenPxRange();
	float inner = width * (d_inner - u_threshold) + 0.5 + u_out_bias;
	float outer = width * (d_outer - u_threshold + u_outline_width_relative) + 0.5 + u_out_bias + u_outline_width_absolute;

	float inner_opacity = clamp(inner, 0.0, 1.0);
	vec4 inner_color = vec4(1, 1, 1, 1);
	float outer_opacity = clamp(outer, 0.0, 1.0);
	vec4 outer_color = vec4(1.00, 0, 0, 1);
	  
	vec4 color = (inner_color * inner_opacity) + (outer_color * (outer_opacity - inner_opacity));
	if(color.a <= 0.6)
        discard;
	outColor = color;*/

	// apply some lighting (hard coded angle)
	/*float u_gradient = 0.15;
	vec2 normal = normalize(vec3(dFdx(d_inner), dFdy(d_inner), 0.01)).xy;
	float light = 0.5 * (1.0 + dot(normal, normalize(vec2(-0.3, -0.5))));
	inner_color = mix(inner_color, vec4(light, light, light, 1),
						smoothstep(u_gradient + u_threshold, u_threshold, d_inner));

	vec4 color = (inner_color * inner_opacity) + (outer_color * (outer_opacity - inner_opacity));
	
	if(color.a <= 0.6)
        discard;
	
	outColor = color;*/
	
	
	
	/*vec2 dxdy = fwidth(v_texCoord.xy) * vec2(imageSize);
    float dist = getDistance() + min(0.1, 0.5 - (1.0/15.0))- 0.5;
	
	float opacity = clamp(dist * distanceRange / length(dxdy) + 0.5, 0.0, 1.0);
	outColor = vec4(v_color.rgb, opacity * v_color.a);*/
	
	float shadowSoftness = 1.0;
	vec3 msd = texture(u_texture, vec3(v_texCoord, v_layer)).rgb;
	vec2 msdfSize = textureSize(u_texture, 0).xy;
	
	float pxSize = min(0.5/pxRange*(fwidth(v_texCoord.x)*msdfSize.x+fwidth(v_texCoord.y)*msdfSize.y), 0.25);

    float sd = 2.0*median(msd.r, msd.g, msd.b)-1.0 + thickness;
	
	float inside = linearstep(-border-pxSize, -border+pxSize, sd);
    float outsideBorder = border > 0.0 
        ? linearstep(+border-pxSize, +border+pxSize, sd) 
        : 1.0;

    vec4 fg = vec4(mix(borderColor, mix(bottomColor, topColor, v_texCoord.y),
                       outsideBorder), 
                   inside);
				   
	msd = texture(u_texture, vec3(v_texCoord - shadowVector, v_layer)).rgb;
	
	sd = 2.0*median(msd.r, msd.g, msd.b)-1.0 + border+thickness;
    float shadow = shadowOpacity*linearstep(-shadowSoftness-pxSize,
                                            +shadowSoftness+pxSize, sd);
											
	outColor = vec4(mix(vec3(0.0), fg.rgb, fg.a), 
                     shadow-shadow*fg.a+fg.a);
    outColor = mix(outColor, shadowColor, (1.0 - inside) * shadow);
   
} 