#version 430 core

const int DepthSamples = 32;
const float InvDepthSamples = 1.0 / DepthSamples;

in vec4 geomDepthCoord;
in vec3 geomLightDir;
in vec3 geomViewDir;
in vec3 geomNormal;

uniform sampler2D DepthMap;
uniform vec2 DepthOffs[32];


out vec3 fragColor;

void main(){

	float LightDist = geomDepthCoord.z/geomDepthCoord.w;
	vec3 Normal = normalize(geomNormal);
	vec3 LightDir = normalize(geomLightDir);
	vec3 LightRefl  = reflect(-LightDir, Normal);
	vec3 ViewDir = normalize(geomViewDir);

	float inv_w = 1.0/geomDepthCoord.w;

	float Depth = 0.0;
	for(int s=0; s!=DepthSamples; ++s){

        vec2 SampleCoord = DepthOffs[s]+geomDepthCoord.xy;
        SampleCoord *= inv_w;
        SampleCoord *= 0.5;
        SampleCoord += 0.5;
        float Sample = texture(DepthMap, SampleCoord).r;
        if(Sample < 0.95)
			Depth += Sample;
        else Depth += 0.5;
    }
	
    Depth *= InvDepthSamples;

    float Ambi = 0.15;
    float BkLt = (dot(-LightDir, ViewDir)+3.0)*0.25;
    float SuSS = pow(abs(Depth-LightDist), 2.0)*BkLt*1.2;
    float Shdw = min(pow(abs(Depth-LightDist)*2.0, 8.0), 1.0);
    float Diff  = sqrt(max(dot(LightDir, Normal)+0.1, 0.0))*0.4;
    float Spec  = pow(max(dot(LightRefl, ViewDir), 0.0), 64.0);
    vec3 Color = vec3(0.2, 0.9, 0.7);
    fragColor = (Ambi + Shdw*Diff + SuSS) * Color;
    fragColor += Shdw*Spec * vec3(1.0, 1.0, 1.0);
	
	fragColor = vec3(1.0, 0.0, 0.0);
}