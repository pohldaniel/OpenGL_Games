#version 410 core                                                                      

uniform sampler2D u_texture;                                                        

in vec2 texCoord;     

out vec4 color;                                                               

void main(){                                                                                 
    color = texture(u_texture, texCoord);                                                                                                                  
}
