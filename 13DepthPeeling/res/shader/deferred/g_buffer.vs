#version 430 core                                                                    
                                                                                    
layout(location = 0) in vec3 i_position;
layout(location = 1) in vec2 i_texCoord;
layout(location = 2) in vec3 i_normal;
layout(location = 3) in vec3 i_tangent;
layout(location = 4) in vec3 i_bitangent;                                               

uniform mat4 u_projection;
uniform mat4 u_view;
uniform mat4 u_model = mat4(1.0);

out vec2 v_texCoord;                                                                     
out vec3 v_normal;                                                                
out vec3 v_tangent;
out vec3 v_bitangent;

void main() { 
  
	v_texCoord	= i_texCoord;
	v_normal	= i_normal;
	v_tangent	= i_tangent;
	v_bitangent	= i_bitangent;
	
	gl_Position = u_projection * u_view * u_model * vec4(i_position, 1.0f);
}