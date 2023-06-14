#pragma once

#include "engine/MeshObject/MeshQuad.h"
#include "engine/Texture.h"
#include "engine/Shader.h"
#include "engine/VolumeBuffer.h"
#include "engine/Framebuffer.h"
#include "engine/Camera.h"
#include "engine/Clock.h"

#include "Light.h"
#include "Sky.h"

//#define COMPUTE
#define INT_CEIL(n,d) (int)ceil((float)n/d)

enum cloudsTextureNames { fragColor, bloom, alphaness, cloudDistance };

class CloudsModel {

public:
	friend class VolumetricClouds;

	CloudsModel(unsigned int width, unsigned int height, const Light& light);
	~CloudsModel();

	void update();
	void draw(const Camera& camera, const Sky& sky);
	
	Shader *m_weatherShaderComp, *m_weatherShader, *m_raymarcher, *m_post;

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
	void initVariables();
	void initShaders();
	void resize(unsigned int width, unsigned int height);
	unsigned int getPostTexture();
	const Texture& getColorTexture(cloudsTextureNames name);

	VolumeBuffer* m_volumeBuffer = nullptr;
	Framebuffer m_weatherMap;
	
	MeshQuad m_quad;
	Texture m_textureSet[4];
	Clock m_clock;
	unsigned int m_width;
	unsigned int m_height;
	Framebuffer postBuffer;

	const Light& light;
};