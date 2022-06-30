#version 330

struct TerrainRegion{
    float min;
    float max;
};

in vec2 texCoord;
in vec4 normal;

out vec4 outColor;

uniform TerrainRegion region1;
uniform TerrainRegion region2;
uniform TerrainRegion region3;
uniform TerrainRegion region4;

uniform sampler2DArray regions;

uniform vec4 lightDir;

vec4 GenerateTerrainColor(){
    vec4 terrainColor = vec4(0.0, 0.0, 0.0, 1.0);
    float height = normal.w;
    float regionMin = 0.0;
    float regionMax = 0.0;
    float regionRange = 0.0;
    float regionWeight = 0.0;
    
    // Terrain region 1.
    regionMin = region1.min;
    regionMax = region1.max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
	terrainColor += regionWeight * texture(regions, vec3(texCoord, 0));

    // Terrain region 2.
    regionMin = region2.min;
    regionMax = region2.max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
	terrainColor += regionWeight * texture(regions, vec3(texCoord, 1));

    // Terrain region 3.
    regionMin = region3.min;
    regionMax = region3.max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
	terrainColor += regionWeight * texture(regions, vec3(texCoord, 2));

    // Terrain region 4.
    regionMin = region4.min;
    regionMax = region4.max;
    regionRange = regionMax - regionMin;
    regionWeight = (regionRange - abs(height - regionMax)) / regionRange;
    regionWeight = max(0.0, regionWeight);
	terrainColor += regionWeight * texture(regions, vec3(texCoord, 3));

    return terrainColor;
}

void main(void) {

	vec3 n = normalize(normal.xyz);
    float nDotL = max(0.0, dot(n, lightDir.xyz));
	
	vec4 ambient = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    vec4 diffuse = vec4(1.0f, 1.0f, 1.0f, 1.0f) * nDotL;
    vec4 color =  ambient + diffuse;   
	
	outColor = color * GenerateTerrainColor();
}
