#pragma once
#include "engine/Texture.h"
#include "engine/Shader.h"
#include "engine/VolumeBuffer.h"
#include "engine/Framebuffer.h"

#define INT_CEIL(n,d) (int)ceil((float)n/d)
//#define COMPUTE

class CloudsModel2 {

public:
	friend class VolumetricClouds;

	CloudsModel2();
	~CloudsModel2();

	void update();

	Shader* m_weatherShaderComp, m_weatherShader;

	float coverage, cloudSpeed, crispiness, curliness, density, absorption;
	float earthRadius, sphereInnerRadius, sphereOuterRadius;
	float perlinFrequency;
	bool enableGodRays;
	bool enablePowder;
	bool postProcess;

	Vector3f cloudColorTop, cloudColorBottom;

	Vector3f seed, oldSeed, sceneSeed;
	unsigned int perlinTex = 0, worley32 = 0, weatherTex = 0;

	void generateWeatherMap();
	void generateModelTextures();
	void generateModelTextures2();
	void initVariables();
	void initShaders();

	VolumeBuffer* m_volumeBuffer = nullptr;
	Framebuffer m_weatherMap;
};