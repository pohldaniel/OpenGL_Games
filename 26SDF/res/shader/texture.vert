#version 430 core

layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;


layout(std140, binding = 0) uniform GlobalUniforms
{
    mat4 view_proj;
    vec4 cam_pos;
    int  num_instances;
};

uniform mat4 u_model = mat4(1.0);


out vec2 texCoord;
out vec3 normal;

out vec3 FS_IN_WorldPos;
out vec3 FS_IN_Normal;
out vec2 FS_IN_UV;
out vec4 FS_IN_NDCFragPos;

void main(void){
		
	vec4 world_pos   = u_model * vec4(i_position, 1.0); ;
	texCoord = i_texCoord; 
	normal = i_normal;
	
	FS_IN_WorldPos   = world_pos.xyz;
    FS_IN_Normal     = normalize(normalize(mat3(u_model) * i_normal));
    FS_IN_UV         = i_texCoord;
    FS_IN_NDCFragPos = view_proj * world_pos;
		
	gl_Position = view_proj * world_pos;	
}