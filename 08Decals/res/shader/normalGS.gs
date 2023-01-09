#version 410 core

const float MAGNITUDE = 10.4;

layout (triangles) in;
layout (line_strip, max_vertices = 18) out;

in vec3 normal[];
in vec3 tangent[];
in vec3 bitangent[];

out vec4 v_color;

uniform mat4 u_projection;

void GenerateNormal(int index){

    gl_Position = u_projection * gl_in[index].gl_Position;
	v_color = vec4(1.0, 0.0, 0.0, 1.0);
    EmitVertex();
    gl_Position = u_projection * (gl_in[index].gl_Position + vec4(normal[index], 0.0) * MAGNITUDE);
	v_color = vec4(1.0, 0.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void GenerateTangent(int index){

    gl_Position = u_projection * gl_in[index].gl_Position;
	v_color = vec4(0.0, 1.0, 0.0, 1.0);
    EmitVertex();
    gl_Position = u_projection * (gl_in[index].gl_Position + vec4(tangent[index], 0.0) * MAGNITUDE);
	v_color = vec4(0.0, 1.0, 0.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void GenerateBiTangent(int index){

    gl_Position = u_projection * gl_in[index].gl_Position;
	v_color = vec4(0.0, 0.0, 1.0, 1.0);
    EmitVertex();
    gl_Position = u_projection * (gl_in[index].gl_Position + vec4(bitangent[index], 0.0) * MAGNITUDE);
	v_color = vec4(0.0, 0.0, 1.0, 1.0);
    EmitVertex();
    EndPrimitive();
}

void main(){
    GenerateNormal(0);
    GenerateNormal(1);
    GenerateNormal(2);
	
	GenerateTangent(0); 
    GenerateTangent(1); 
    GenerateTangent(2);
	
	GenerateBiTangent(0); 
    GenerateBiTangent(1); 
    GenerateBiTangent(2);
}  