#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_projection;
uniform mat4 u_cameraMatrices[6];
uniform vec4 u_colors[6];

in vec2 texCoord[];
in vec3 normal[];

out vec2 v_texCoord;
out vec3 v_normal;
out vec4 v_color;

void main(){

    for(int face = 0; face < 6; face++){
	
        gl_Layer = face;
        for(int i = 0; i < 3; i++){
 
            gl_Position = u_projection * u_cameraMatrices[face] * gl_in[i].gl_Position;
			v_texCoord = texCoord[i];			
			v_normal = mat3(transpose(inverse(u_cameraMatrices[face]))) * normal[i];
			v_color = u_colors[face];
	
            EmitVertex();
        }
        EndPrimitive();
    }
}  