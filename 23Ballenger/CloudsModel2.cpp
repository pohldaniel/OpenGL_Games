#include <GL/glew.h>
#include "CloudsModel2.h"
#include "Globals.h"

CloudsModel2::CloudsModel2() {
	sceneSeed = Vector3f(0.0f, 0.0f, 0.0f);
	m_weatherMap.create(1024, 1024);
	m_weatherMap.attachTexture(weatherTex, Attachment::COLOR, Target::TEXTURE2D, 0);
	initVariables();
	generateModelTextures2();
}

void CloudsModel2::initShaders() {
#ifdef COMPUTE
	m_weatherShaderComp = new Shader("shaders/weather.comp");
#else
	m_weatherShader = new Shader("res/weather.vert", "res/weather.frag");
#endif
}

void CloudsModel2::generateModelTextures2() {
#ifdef COMPUTE
	
	Texture::CreateTexture3D(this->perlinTex, 128, 128, 128, GL_RGBA8, GL_RGBA, GL_FLOAT);
	Texture::SetWrapMode(this->perlinTex, GL_REPEAT, GL_TEXTURE_3D);
	Texture::SetFilter(this->perlinTex, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	
	Shader perlin("shaders/perlinworley.comp");
	perlin.use();
	glBindImageTexture(0, this->perlinTex, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
	glDispatchCompute(INT_CEIL(128, 4), INT_CEIL(128, 4), INT_CEIL(128, 4));
	perlin.unuse();

	Texture::CreateTexture3D(this->worley32, 32, 32, 32, GL_RGBA8, GL_RGBA, GL_FLOAT);
	Texture::SetWrapMode(this->worley32, GL_REPEAT, GL_TEXTURE_3D);
	Texture::SetFilter(this->worley32, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);

	Shader worley("shaders/worley.comp");
	worley.use();
	glBindImageTexture(0, this->worley32, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA8);
	glDispatchCompute(INT_CEIL(32, 4), INT_CEIL(32, 4), INT_CEIL(32, 4));
	worley.unuse();

#else
	if (!VolumeBuffer::LoadVolumeFromRaw("res/noise/worley.raw", worley32, 32, 32, 32)) {
		m_volumeBuffer = new VolumeBuffer(GL_RGBA8, 32, 32, 32);
		m_volumeBuffer->setFiltering(GL_LINEAR_MIPMAP_LINEAR);
		m_volumeBuffer->setWrapMode(GL_REPEAT);
		m_volumeBuffer->setShader(Globals::shaderManager.getAssetPointer("worley"));
		m_volumeBuffer->draw();
		m_volumeBuffer->writeVolumeToRaw("res/noise/worley.raw");
		m_volumeBuffer->getVolume(worley32);
	}else {
		Texture::SetWrapMode(worley32, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(worley32, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	}

	if (!VolumeBuffer::LoadVolumeFromRaw("res/noise/perlinworley.raw", perlinTex, 128, 128, 128)) {

		if (m_volumeBuffer) {
			m_volumeBuffer->resize(128, 128, 128);
		}
		else {
			m_volumeBuffer = new VolumeBuffer(GL_RGBA8, 128, 128, 128);
			m_volumeBuffer->setFiltering(GL_LINEAR_MIPMAP_LINEAR);
			m_volumeBuffer->setWrapMode(GL_REPEAT);
		}
		m_volumeBuffer->setShader(Globals::shaderManager.getAssetPointer("perlinworley"));
		m_volumeBuffer->draw();
		m_volumeBuffer->writeVolumeToRaw("res/noise/perlinworley.raw");
		m_volumeBuffer->getVolume(perlinTex);
	}else {
		Texture::SetWrapMode(perlinTex, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(perlinTex, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	}

#endif

	if (!weatherTex) {
		Texture::CreateTexture2D(this->weatherTex, 1024, 1024, GL_RGBA32F, GL_RGBA, GL_FLOAT);
		Texture::SetWrapMode(this->weatherTex, GL_REPEAT);
		generateWeatherMap();
		seed = sceneSeed;
		oldSeed = seed;
	}
}

void CloudsModel2::generateModelTextures() {
	
	if (!weatherTex) {
		Texture::CreateTexture2D(this->weatherTex, 1024, 1024, GL_RGBA32F, GL_RGBA, GL_FLOAT);
		Texture::SetWrapMode(this->weatherTex, GL_REPEAT);
		generateWeatherMap();

		seed = sceneSeed;
		oldSeed = seed;
	}
}

CloudsModel2::~CloudsModel2(){
	delete m_weatherShaderComp;
}

void CloudsModel2::update(){
	seed = sceneSeed;
	if (seed != oldSeed) {
		generateWeatherMap();
		oldSeed = seed;
	}
}

void CloudsModel2::generateWeatherMap() {	

#ifdef COMPUTE
	m_weatherShaderComp->use();
	m_weatherShaderComp->loadVector("seed", sceneSeed);
	m_weatherShaderComp->loadFloat("perlinFrequency", perlinFrequency);
	glBindImageTexture(0, weatherTex, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glDispatchCompute(INT_CEIL(1024, 8), INT_CEIL(1024, 8), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	m_weatherShaderComp->unuse();
#else
	m_weatherMap.bind();

	m_weatherMap.unbind();
#endif
}

void CloudsModel2::initVariables() {
	cloudSpeed = 450.0;
	coverage = 0.45;
	crispiness = 40.;
	curliness = .1;
	density = 0.02;
	absorption = 0.35;

	earthRadius = 600000.0;
	sphereInnerRadius = 5000.0;
	sphereOuterRadius = 17000.0;

	perlinFrequency = 0.8;

	enableGodRays = false;
	enablePowder = false;
	postProcess = true;

	seed = Vector3f(0.0, 0.0, 0.0);
	oldSeed = Vector3f(0.0, 0.0, 0.0);

	cloudColorTop = (Vector3f(169., 149., 149.)*(1.5f / 255.f));
	cloudColorBottom = (Vector3f(65., 70., 80.)*(1.5f / 255.f));

	weatherTex = 0;
	perlinTex = 0;
	worley32 = 0;
}
