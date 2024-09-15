#version 430 core


uniform sampler2D BackgroundSampler;
uniform sampler2D ParticlesSampler;

in vec2 v_texCoord;

out vec4 colorOut;

void main()
{
    vec2 tc = v_texCoord;
    vec4 dest = texture2D(BackgroundSampler, tc);
    vec4 src = texture2D(ParticlesSampler, tc);
    float a = 1.0 - src.a;
    colorOut.rgb = src.rgb * a + dest.rgb * (1.0 - a);
    colorOut.a = 1.0;
	
	//colorOut = dest;
}
