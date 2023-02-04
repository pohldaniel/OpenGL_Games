#version 410 core

in vec2 vTexCoord;

uniform sampler2D TexSampler;

out vec4 color;

void main(){

	vec3 sum = vec3(0.0,0.0,0.0);
	vec2 texcoord;
	texcoord = vTexCoord + vec2(-0.00912868, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.012595;

	texcoord = vTexCoord + vec2(-0.00668976, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.0513832;
	
	texcoord = vTexCoord + vec2(-0.00425498, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.135928;
		
	texcoord = vTexCoord + vec2(-0.00182304, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.233308;
	
	texcoord = vTexCoord + vec2(0.000607643, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.259924;
	
	texcoord = vTexCoord + vec2(0.00303875, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.187971;
	
	texcoord = vTexCoord + vec2(0.00547193, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.088219;
	
	texcoord = vTexCoord + vec2(0.00790863, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.0268563;
	
	texcoord = vTexCoord + vec2(0.01, 0.000000);
	sum += texture2D(TexSampler, texcoord).rgb * 0.00381553;

    color = vec4(sum, 1.0);
}