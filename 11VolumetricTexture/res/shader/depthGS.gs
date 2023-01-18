#version 410 core

const int NUM_CASCADES = 10;

layout(triangles, invocations = 4 /*NUM_CASCADES*/) in;
layout(triangle_strip, max_vertices = 3) out;

in vec2 texCoord[];

out vec2 v_texCoord;

uniform mat4 u_viewShadows[NUM_CASCADES];
uniform mat4 u_projectionShadows[NUM_CASCADES];
uniform int u_numCascades = 0;

void main(void){

	//gl_Layer = gl_InvocationID;
	
	//for(int j = 0; j < NUM_CASCADES; ++j){
		gl_Layer = u_numCascades;
		for (int i = 0; i < 3; ++i){
			gl_Position = u_projectionShadows[u_numCascades] * u_viewShadows[u_numCascades] * gl_in[i].gl_Position;
			v_texCoord = texCoord[i];
			EmitVertex();
		}
		EndPrimitive();
	//}
}