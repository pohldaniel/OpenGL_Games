#version 410 core

layout (points) in;
layout (triangle_strip, max_vertices = 4) out;

in vec3 eyePosition[];
in vec3 color[];

uniform mat4 u_projection;

out vec2 texCoord;
out vec3 lightPos;
out vec3 lightColor;

//
//  Each light casts itself through roughly a 1 ft radius sphere. To accomplish this,
// a sprite is created that is centered at the light position, and extends 1ft in the
// positive and negative x and y directions. The sprite is pulled forward by 1 ft in
// eye-space depth to avoid any bad depth rejects. Finally, the depth is clamped to 
// the near plane to ensure that the light does not get culled while the light may
// still be influencing the scene.
// 

void main() {
	
	//lower left
	gl_Position = u_projection * vec4( eyePosition[0] + vec3( -1.0, -1.0, 1.0), 1.0);
	gl_Position /= gl_Position.w;
	gl_Position.z = (gl_Position.z > -1.0) ? gl_Position.z : -1.0; 
	texCoord = (gl_Position.xy) * 0.5 + 0.5;
	lightPos = eyePosition[0];
	lightColor = color[0];
	
	EmitVertex();
	
	//lower right
	gl_Position = u_projection * vec4( eyePosition[0] + vec3( 1.0, -1.0, 1.0), 1.0);
	gl_Position /= gl_Position.w;
	gl_Position.z = (gl_Position.z > -1.0) ? gl_Position.z : -1.0;
	texCoord = (gl_Position.xy) * 0.5 + 0.5;
	lightPos = eyePosition[0];
	lightColor = color[0];
	
	EmitVertex();
	
	//upper left
	gl_Position = u_projection * vec4( eyePosition[0] + vec3( -1.0, 1.0, 1.0), 1.0);
	gl_Position /= gl_Position.w;
	gl_Position.z = (gl_Position.z > -1.0) ? gl_Position.z : -1.0;
	texCoord = (gl_Position.xy) * 0.5 + 0.5;
	lightPos = eyePosition[0];
	lightColor = color[0];
	
	EmitVertex();
	
	//upper right
	gl_Position = u_projection * vec4( eyePosition[0] + vec3( 1.0, 1.0, 1.0), 1.0);
	gl_Position /= gl_Position.w;
	gl_Position.z = (gl_Position.z > -1.0) ? gl_Position.z : -1.0;
	texCoord = (gl_Position.xy) * 0.5 + 0.5;
	lightPos = eyePosition[0];
	lightColor = color[0];
	
	EmitVertex();
	
	EndPrimitive();
}