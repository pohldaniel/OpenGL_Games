#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec4 hPos[];
in vec3 color[];
in float ptSize[];

out vec3 oColor;
out vec2 coord;

void main() {
    float scale = hPos[0].w * ptSize[0];
	
	gl_Position = hPos[0] + vec4( -0.5/400.0, -0.5/300.0, 0.0, 0.0) * scale;
	coord = vec2( -1.0, -1.0);
	oColor = color[0];
	EmitVertex();
	
	gl_Position = hPos[0] + vec4( 0.5/400.0, -0.5/300.0, 0.0, 0.0) * scale;
	coord = vec2( 1.0, -1.0);
	oColor = color[0];
	EmitVertex();
	
	gl_Position = hPos[0] + vec4( -0.5/400.0, 0.5/300.0, 0.0, 0.0) * scale;
	coord = vec2( -1.0, 1.0);
	oColor = color[0];
	EmitVertex();
	
	gl_Position = hPos[0] + vec4( 0.5/400.0, 0.5/300.0, 0.0, 0.0) * scale;
	coord = vec2( 1.0, 1.0);
	oColor = color[0];
	EmitVertex();
}