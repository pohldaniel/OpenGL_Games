#version 410 core
layout (triangles) in;
layout (triangle_strip, max_vertices=18) out;

uniform mat4 u_projection;
uniform mat4 u_cameraMatrices[6];
uniform vec4 u_colors[6];

in vec2 v_texCoord[];
in vec3 v_pos_model[];
in vec4 v_pos_eye[];
in vec3 v_normal_model[];

out vec2 texCoord;
out vec3 pos_model;
out vec3 pos_eye;
out vec3 normal_model;
out vec3 normal_eye;
out vec4 v_color;

void main(){

    for(int face = 0; face < 6; face++){
	
        gl_Layer = face;
        for(int i = 0; i < 3; i++){
 
            gl_Position = u_projection * u_cameraMatrices[face] * gl_in[i].gl_Position;
			texCoord = v_texCoord[i];
			pos_model = v_pos_model[i];
			pos_eye = vec3(u_cameraMatrices[face] * v_pos_eye[i]);
			normal_model = v_normal_model[i];
			normal_eye = mat3(transpose(inverse(u_cameraMatrices[face]))) * v_normal_model[i];
			v_color = u_colors[face];
	
            EmitVertex();
        }
        EndPrimitive();
    }
}  