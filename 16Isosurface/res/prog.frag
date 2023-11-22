#version 410 core

in vec3 v_normal;
in vec4 v_color;

uniform vec3 L = vec3(0.0, 0.0, -1.0);
uniform bool u_withFrag = true;

out vec4 outColor;

void main(){
	if(u_withFrag){
		vec3 N = normalize(v_normal);
		vec3 materials[2] = vec3[2](vec3(1.0, 1.0, 1.0), vec3(0.0, 0.0, 0.5));
	
		float nDotL = dot( N, L);
		outColor = vec4(abs(nDotL) * materials[int(nDotL < 0.0)], 0.1);
		//outColor vec4(vec3(N*0.5+0.5), 1);
	}else{
		outColor = v_color;
	}
}  