#version 430 core


uniform sampler2D BackgroundSampler;
uniform sampler2D ParticlesSampler;

in vec2 v_texCoord;

void main()
{
    vec2 tc = v_texCoord;
    vec4 dest = texture2D(BackgroundSampler, tc);
    vec4 src = texture2D(ParticlesSampler, tc);
    float a = 1.0 - src.a;
    gl_FragColor.rgb = src.rgb * a + dest.rgb * (1.0 - a);
    gl_FragColor.a = 1.0;
	
	//gl_FragColor = dest;
}
