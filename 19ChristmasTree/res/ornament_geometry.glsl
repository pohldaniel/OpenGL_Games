#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 position[];
in vec4 mat[];

uniform mat4 u_projection;

out vec2 texCoord;
out vec3 eyePosition;
out vec4 material;


void main() {
	float radius = 2.5f / 12.0f;
	
	//lower left
	eyePosition = position[0] + vec3( -radius, -radius, 0.0);
	gl_Position = u_projection * vec4( eyePosition, 1.0);
	texCoord = vec2( 0.0, 0.0);
	material = mat[0];
	
	EmitVertex();
	
	//lower right
	eyePosition = position[0] + vec3( radius, -radius, 0.0);
	gl_Position = u_projection * vec4( eyePosition, 1.0);
	texCoord = vec2( 1.0, 0.0);
	material = mat[0];
	
	EmitVertex();
	
	//upper left
	eyePosition = position[0] + vec3( -radius, radius, 0.0);
	gl_Position = u_projection * vec4( eyePosition, 1.0);
	texCoord = vec2( 0.0, 1.0);
	material = mat[0];
	
	EmitVertex();
	
	//upper right
	eyePosition = position[0] + vec3( radius, radius, 0.0);
	gl_Position = u_projection * vec4( eyePosition, 1.0);
	texCoord = vec2( 1.0, 1.0);
	material = mat[0];
	
	EmitVertex();
	
	EndPrimitive();
}