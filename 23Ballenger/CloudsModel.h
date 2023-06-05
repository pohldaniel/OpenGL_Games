#pragma once
#include "engine/Texture.h"
#include "engine/Shader.h"

#define INT_CEIL(n,d) (int)ceil((float)n/d)

//CloudsModel is responsible to collect the attributes and shaders that will be needed to render the volumetric clouds. Also, it creates the noises which models the clouds.
class CloudsModel {

public:
	friend class VolumetricClouds;

	CloudsModel();
	~CloudsModel();
	
	void update();
	void setGui();

	Shader *volumetricCloudsShader, *weatherShader;

	float coverage, cloudSpeed, crispiness, curliness, density, absorption;
	float earthRadius, sphereInnerRadius, sphereOuterRadius;
	float perlinFrequency;
	bool enableGodRays;
	bool enablePowder;
	bool postProcess;

	Vector3f cloudColorTop, cloudColorBottom;

	Vector3f seed, oldSeed, sceneSeed;
	unsigned int perlinTex, worley32, weatherTex;

	void generateWeatherMap();
	void generateModelTextures();
	void initVariables();
	void initShaders();
};

