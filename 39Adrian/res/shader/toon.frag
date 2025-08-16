#version 410 core

in vec2 v_texCoord;
in vec4 v_color;

uniform sampler2D u_color;

out vec4 outColor;

void main(){

	vec4 texel = texture2D(u_color, v_texCoord);
	mat3 gx = mat3(-1.0f, -2.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 2.0f, 1.0f);
    mat3 gy = mat3(1.0f, 0.0f, -1.0f, 2.0f, 0.0f, -2.0f, 1.0f, 0.0f, -1.0f);
	int i, j;
	vec4 ones = vec4(1.0, 1.0, 1.0, 1.0); 
    vec4 zeros = vec4(0.0, 0.0, 0.0, 0.0); 
	
	vec4 sx = vec4(0, 0, 0, 0);
    vec4 sy = vec4(0, 0, 0, 0);
    float contrib = 0;
	
	for(i = -1; i < 2; i++){
        for(j = -1; j < 2; j++){
            vec2 coord = v_texCoord + vec2(i/1024.0, j/768.0);
            sx += gx[i+1][j+1] * texture2D(u_color, coord.st);
            sy += gy[i+1][j+1] * texture2D(u_color, coord.st);
        }
    }
	
	float sum = dot(abs(sx), abs(sy));
    if(sum > 0.1)
        outColor = ones * 0.2; 
    else
        outColor = texel + ones * 0.2; 
}