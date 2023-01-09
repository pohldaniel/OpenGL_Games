#version 410 core

uniform sampler2D u_texture;

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_worldPos;

out vec3 color;

void main(void){

	vec3  light_pos = vec3(200.0, 200.0, 200.0);
    vec3  n         = normalize(v_normal);
    vec3  l         = normalize(light_pos - v_worldPos);
    float lambert   = max(0.0f, dot(n, l));
    vec3  diffuse   = texture(u_texture, v_texCoord).xyz;
    vec3  ambient   = diffuse * 0.03;
	
    color = diffuse * lambert + ambient;	
}
