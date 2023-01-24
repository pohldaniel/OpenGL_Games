#version 430 core 
                                                                        
layout(location = 0) out vec3 FS_OUT_Albedo;
layout(location = 1) out vec3 FS_OUT_Normal;
layout(location = 2) out vec3 FS_OUT_SrcNormal;
layout(location = 3) out vec3 FS_OUT_Tangent;
layout(location = 4) out vec3 FS_OUT_Bitangent;   

in vec2 v_texCoord;                                                                     
in vec3 v_normal;                                                                
in vec3 v_tangent;
in vec3 v_bitangent;
										
uniform sampler2D s_Albedo;               
uniform sampler2D s_Normal; 

vec3 get_normal_from_map(vec3 tangent, vec3 bitangent, vec3 normal, vec2 tex_coord, sampler2D normal_map){
    // Create TBN matrix.
    mat3 TBN = mat3(normalize(tangent), normalize(bitangent), normalize(normal));

    // Sample tangent space normal vector from normal map and remap it from [0, 1] to [-1, 1] range.
    vec3 n = texture(normal_map, tex_coord).xyz;

    n.y = 1.0 - n.y;

    n = normalize(n * 2.0 - 1.0);

    // Multiple vector by the TBN matrix to transform the normal from tangent space to world space.
    n = normalize(TBN * n);

    return n;
}
										
void main()	{											
	vec4 diffuse = texture(s_Albedo, v_texCoord);

    vec3 N = normalize(v_normal);
    vec3 T = normalize(v_tangent);
    vec3 B = normalize(v_bitangent);

    FS_OUT_Albedo    = diffuse.xyz;
    FS_OUT_Normal    = get_normal_from_map(T, B, N, v_texCoord, s_Normal);
    FS_OUT_SrcNormal = N;
    FS_OUT_Tangent   = T;
    FS_OUT_Bitangent = B;			
}