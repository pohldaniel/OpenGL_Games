#version 410 core

const int NUM_CASCADES = 4;

layout(triangles, invocations = 4) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 u_viewShadows[NUM_CASCADES];
uniform mat4 u_projectionShadows[NUM_CASCADES];

uniform mat4 u_projection;
uniform mat4 u_modelView = mat4(1.0);

void main(void){

	gl_Layer = gl_InvocationID;
	for (int i = 0; i < 3; ++i){
        gl_Position = u_projectionShadows[gl_InvocationID] * u_viewShadows[gl_InvocationID] * gl_in[i].gl_Position;
		//gl_Position = gl_in[i].gl_Position;
        EmitVertex();
    }
    EndPrimitive();
	
	//for(int i = 0; i < 3; i++){
    //    gl_Position = u_projection * u_modelView * gl_in[i].gl_Position;
		
    //    EmitVertex();
    //}	
	//EndPrimitive();
}