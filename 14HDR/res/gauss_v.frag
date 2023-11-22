#version 410 core

in vec2 vTexCoord;

uniform sampler2D TexSampler;

out vec4 color;

void main(){

	vec3 sum = vec3(0.0,0.0,0.0);
	vec2 texcoord;
	texcoord = vTexCoord + vec2(0.000000, -0.0162288);
	sum += texture2D(TexSampler, texcoord).rgb * 0.012595;

	texcoord = vTexCoord + vec2(0.000000, -0.0118929);
	sum += texture2D(TexSampler, texcoord).rgb * 0.0513832;
	
	texcoord = vTexCoord + vec2(0.000000, -0.00756441);
	sum += texture2D(TexSampler, texcoord).rgb * 0.135928;
		
	texcoord = vTexCoord + vec2(0.000000, -0.00324095);
	sum += texture2D(TexSampler, texcoord).rgb * 0.233308;
	
	texcoord = vTexCoord + vec2(0.000000, 0.00108025);
	sum += texture2D(TexSampler, texcoord).rgb * 0.259924;
	
	texcoord = vTexCoord + vec2(0.000000, 0.00540222);
	sum += texture2D(TexSampler, texcoord).rgb * 0.187971;
	
	texcoord = vTexCoord + vec2(0.000000, 0.00972787);
	sum += texture2D(TexSampler, texcoord).rgb * 0.088219;
	
	texcoord = vTexCoord + vec2(0.000000, 0.0140598);
	sum += texture2D(TexSampler, texcoord).rgb * 0.0268563;
	
	texcoord = vTexCoord + vec2(0.000000, 0.0177778);
	sum += texture2D(TexSampler, texcoord).rgb * 0.00381553;

    color = vec4(sum, 1.0);
}