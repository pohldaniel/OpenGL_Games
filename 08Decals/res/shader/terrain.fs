#version 410 core

const uint NUM_CASCADES = 10;

struct Region{
    vec2 border1;
    vec2 border2;
};

struct TerrainRegion{
    float min;
    float max;
};

in vec2 texCoordTiled;
in vec2 texCoord;
in vec4 normal;
in float visibility;

in vec4 scs[NUM_CASCADES];
in float clipSpacePosZ;
flat in uint numCascades;

layout(location = 0) out vec4 outColor;

layout (std140) uniform u_region{
	Region region[2]; //0  -> 32
};

layout (std140) uniform u_tearrainRegion{
	TerrainRegion tearrainRegion[4]; //0  -> 64
}; 

layout (std140) uniform u_regionBorder{
    float region1[2];   //0  -> 32
	float region2[2];	//32 -> 64
	vec4 region34;		//64 -> 80
}; 

uniform sampler2DArray regions;
uniform sampler2DArray blend;
uniform sampler2DArray u_shadowMaps;

uniform sampler2D path;
uniform sampler2D blendMap;

uniform float u_cascadeEndClipSpace[NUM_CASCADES];
uniform bool u_debug = false;

uniform vec4 lightDir;
uniform vec4 fogColor;

uniform bool mode = true;

float getDepthPassSpaceZ(float zWC, float near, float far){

	// Assume standard opengl depth range [0..1]
    float z_n = 2.0 * zWC - 1.0;
    float z_e = (2.0 * near * far) / (far + near + z_n * (near - far));	//[near, far]

	//divided by far to get the range [near/far, 1.0] just for visualisation
	//float z_e = (2.0 * near) / (far + near + z_n * (near - far));	

	return z_e;
}


vec4 GenerateTerrainColor(){
    vec4 terrainColor = vec4(0.0, 0.0, 0.0, 1.0);
    float height = normal.w;
    float regionMin = 0.0;
    float regionMax = 0.0;
    float regionRange = 0.0;
    float regionWeight = 0.0;
    
    // Terrain region 1.
	regionMin = region[0].border1.x;
    regionMax = region[0].border1.y;
	//regionMin = tearrainRegion[0].min;
    //regionMax = tearrainRegion[0].max;
    //regionMin = region1[0];
    //regionMax = region1[1];	
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
	terrainColor += regionWeight * texture(regions, vec3(texCoordTiled, 0));

    // Terrain region 2.
	regionMin = region[0].border2.x;
    regionMax = region[0].border2.y;
	//regionMin = tearrainRegion[1].min;
    //regionMax = tearrainRegion[1].max;
    //regionMin = region2[0];
    //regionMax = region2[1];
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
	terrainColor += regionWeight * texture(regions, vec3(texCoordTiled, 1));

    // Terrain region 3.
	regionMin = region[1].border1.x;
    regionMax = region[1].border1.y;
	//regionMin = tearrainRegion[2].min;
    //regionMax = tearrainRegion[2].max;
    //regionMin = region34.x;
    //regionMax = region34.y;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
	terrainColor += regionWeight * texture(regions, vec3(texCoordTiled, 2));

    // Terrain region 4.
	regionMin = region[1].border2.x;
    regionMax = region[1].border2.y;
	//regionMin = tearrainRegion[3].min;
    //regionMax = tearrainRegion[3].max;
    //regionMin = region34.z;
    //regionMax = region34.w;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
	terrainColor += regionWeight * texture(regions, vec3(texCoordTiled, 3));

    return terrainColor;
}

vec4 GenerateTerrainColorBlended(){
	vec4 blendMapColor = texture(blendMap, texCoord);
	float backTextureAmount = 1 - (blendMapColor.r + blendMapColor.g + blendMapColor.b);
	
	vec4 grassTextureColor = texture(blend, vec3(texCoordTiled, 0)) * backTextureAmount;
	vec4 mudTextureColor = texture(blend, vec3(texCoordTiled, 1)) * blendMapColor.r;
	vec4 flowerTextureColor = texture(blend, vec3(texCoordTiled, 2)) * blendMapColor.g;
	vec4 pathTextureColor = texture(path, texCoordTiled);
	pathTextureColor.a = 1.0f;
	return grassTextureColor + mudTextureColor + flowerTextureColor + pathTextureColor * blendMapColor.b;
}

float CalcShadowFactor(uint cascadeIndex, vec4 sc){
    vec3 ndc = (sc.xyz/sc.w);
	float depth = texture(u_shadowMaps, vec3(ndc.xy, cascadeIndex)).r;

	return ndc.z > depth  ? 0.1 : 1.0;
}

void main(void) {

	float ShadowFactor = 0.0;

	for(uint i = 0; i < numCascades; i++){
		if (clipSpacePosZ <= u_cascadeEndClipSpace[i]) {
            ShadowFactor = CalcShadowFactor(i, scs[i]);	
			break;
        }
    }

	vec3 n = normalize(normal.xyz);
    float nDotL = max(0.0, dot(n, lightDir.xyz));
	
	vec4 ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f) * nDotL;
    vec4 color =  ambient + diffuse;   

	vec4 colorTerrain = mode ? GenerateTerrainColor() : GenerateTerrainColorBlended();

	outColor = color * colorTerrain * ShadowFactor;	
	//outColor = mix(fogColor, outColor, visibility);
	//gl_FragDepth = gl_FragCoord.z;

	//float depth = getDepthPassSpaceZ(gl_FragCoord.z, 1.0, 5000.0) / 5000.0; // divide by far for demonstration
    //outColor = vec4(1.0, 0.0, 0.0, 1.0);
}