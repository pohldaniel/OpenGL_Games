#version 410 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec3 i_color;

uniform mat4 u_projection;
uniform mat4 u_modelView;

out vec3 color;
out vec4 hPos;
out float ptSize;

void main() {
	const float scale = 1.0 / 12.0;
	const vec2 vpScale = vec2(400.0, 300.0);
 	
 	hPos = u_projection * u_modelView * vec4(i_position, 1.0);
	gl_Position = hPos;
	
	vec4 eyePos = u_modelView * vec4(i_position, 1.0);
	
	vec4 p0, p1;
	
	p0 = u_projection * ( eyePos + vec4( 0.125, 0.125, 0.0, 0.0) * scale);
	p1 = u_projection * ( eyePos - vec4( 0.125, 0.125, 0.0, 0.0) * scale); 
	p0 /= p0.w;
	p1 /= p1.w;
	p0.xy *= vpScale;
	p1.xy *= vpScale;
	p0.xy += vpScale;
	p1.xy += vpScale;
	
	float size = distance( p0.xy, p1.xy);
	
	float lightScale = 1.0;
	
	if (size < 1.0) {
	    lightScale = scale;
	    size = 1.0;
	}
	
	ptSize = size;
	
	color = vec3( 128.0 * lightScale * i_color);
}