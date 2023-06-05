#pragma once
#include <glm/glm.hpp>

#include "engine/Shader.h"
#include "_texture.h"

#define INT_CEIL(n,d) (int)ceil((float)n/d)

//CloudsModel is responsible to collect the attributes and shaders that will be needed to render the volumetric clouds. Also, it creates the noises which models the clouds.
class CloudsModel {

public:
	friend class VolumetricClouds;

	CloudsModel();
	~CloudsModel();
	
	// fake implementation, it's needed to let this class being a drawableObject to subscribe to GUI class. must be fixed
	virtual void draw() {};

	virtual void update();
	virtual void setGui();

	Shader *volumetricCloudsShader, *weatherShader;

	float coverage, cloudSpeed, crispiness, curliness, density, absorption;
	float earthRadius, sphereInnerRadius, sphereOuterRadius;
	float perlinFrequency;
	bool enableGodRays;
	bool enablePowder;
	bool postProcess;

	glm::vec3 cloudColorTop, cloudColorBottom;

	glm::vec3 seed, oldSeed, sceneSeed;
	unsigned int perlinTex, worley32, weatherTex;

	void generateWeatherMap();
	void generateModelTextures();
	void initVariables();
	void initShaders();
};

