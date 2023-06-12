#include <GL/glew.h>
#include "CloudsModel.h"
//#include "Globals.h"

CloudsModel::CloudsModel(unsigned int width, unsigned int height, const Light& light)
	: m_width(width),
	  m_height(height),
	  light(light) {

	sceneSeed = Vector3f(0.0f, 0.0f, 0.0f);
	initShaders();
	initVariables();
	m_quad.create(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), true, false, false, 1, 1);

	generateModelTextures();
	m_textureSet[0].createEmptyTexture(m_width, m_height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[1].createEmptyTexture(m_width, m_height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[2].createEmptyTexture(m_width, m_height, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	m_textureSet[3].createEmptyTexture(m_width, m_height, GL_RGBA32F, GL_RGBA, GL_FLOAT);

	m_textureSet[0].setWrapMode(GL_REPEAT);
	m_textureSet[1].setWrapMode(GL_REPEAT);
	m_textureSet[2].setWrapMode(GL_REPEAT);
	m_textureSet[3].setWrapMode(GL_REPEAT);

	postBuffer.create(m_width, m_height);
	postBuffer.attachTexture2D(AttachmentTex::RGBA32F);

	Texture::SetFilter(postBuffer.getColorTexture(0), GL_LINEAR_MIPMAP_LINEAR);

	m_clock.restart();
}

void CloudsModel::initShaders() {
#ifdef COMPUTE
	m_weatherShaderComp = new Shader("shaders/weather.comp");
#else
	m_weatherShader = new Shader("res/weather.vert", "res/weather.frag");
#endif
	m_raymarcher = new Shader("res/volumetric_clouds.comp");
	m_post = new Shader("res/clouds_post.vert", "res/clouds_post.frag");
}

void CloudsModel::generateModelTextures() {

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
		Shader* worley = new Shader("res/worley.vert", "res/worley.frag");
		m_volumeBuffer = new VolumeBuffer(GL_RGBA8, 32, 32, 32);
		m_volumeBuffer->setFiltering(GL_LINEAR);
		m_volumeBuffer->setWrapMode(GL_REPEAT);
		m_volumeBuffer->setShader(worley);
		m_volumeBuffer->draw();
		m_volumeBuffer->writeVolumeToRaw("res/noise/worley.raw");
		m_volumeBuffer->getVolume(worley32);
		delete worley;
	}else {
		Texture::SetWrapMode(worley32, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(worley32, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	}

	if (!VolumeBuffer::LoadVolumeFromRaw("res/noise/perlinworley.raw", perlinTex, 128, 128, 128)) {

		if (m_volumeBuffer) {
			m_volumeBuffer->resize(128, 128, 128);
		}else {
			m_volumeBuffer = new VolumeBuffer(GL_RGBA8, 128, 128, 128);
			m_volumeBuffer->setFiltering(GL_LINEAR);
			m_volumeBuffer->setWrapMode(GL_REPEAT);
		}
		Shader* perlin = new Shader("res/perlinworley.vert", "res/perlinworley.frag");
		m_volumeBuffer->setShader(perlin);
		m_volumeBuffer->draw();
		m_volumeBuffer->writeVolumeToRaw("res/noise/perlinworley.raw");
		m_volumeBuffer->getVolume(perlinTex);
		delete perlin;
	}else {
		Texture::SetWrapMode(perlinTex, GL_REPEAT, GL_TEXTURE_3D);
		Texture::SetFilter(perlinTex, GL_LINEAR_MIPMAP_LINEAR, GL_LINEAR, GL_TEXTURE_3D);
	}

#endif

	Texture::CreateTexture2D(this->weatherTex, 1024, 1024, GL_RGBA32F, GL_RGBA, GL_FLOAT);
	Texture::SetWrapMode(this->weatherTex, GL_REPEAT);
	m_weatherMap.create(1024, 1024);
	m_weatherMap.attachTexture(weatherTex, Attachment::COLOR, Target::TEXTURE2D, 0);
	generateWeatherMap();
	seed = sceneSeed;
	oldSeed = seed;	
}

CloudsModel::~CloudsModel(){
#ifdef COMPUTE
	delete m_weatherShaderComp;
#else
	delete m_weatherShader;
#endif
	delete m_raymarcher;
	delete m_post;
}

void CloudsModel::update(){
	seed = sceneSeed;
	if (seed != oldSeed) {
		generateWeatherMap();
		oldSeed = seed;
	}
}

void CloudsModel::generateWeatherMap() {

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
	m_weatherShader->use();
	m_weatherShader->loadVector("seed", sceneSeed);
	m_weatherShader->loadFloat("perlinFrequency", perlinFrequency);
	m_quad.drawRaw();
	m_weatherShader->unuse();
	m_weatherMap.unbind();
#endif
}

void CloudsModel::initVariables() {
	cloudSpeed = 450.0f;
	coverage = 0.45f;
	crispiness = 40.0f;
	curliness = .1f;
	density = 0.02f;
	absorption = 0.35f;

	earthRadius = 600000.0f;
	sphereInnerRadius = 5000.0f;
	sphereOuterRadius = 17000.0f;

	perlinFrequency = 0.8f;

	enableGodRays = false;
	enablePowder = false;
	postProcess = true;

	seed = Vector3f(0.0f, 0.0f, 0.0f);
	oldSeed = Vector3f(0.0f, 0.0f, 0.0f);

	cloudColorTop = (Vector3f(169.0f, 149.0f, 149.0f)*(1.5f / 255.0f));
	cloudColorBottom = (Vector3f(65.0f, 70.0f, 80.0f)*(1.5f / 255.0f));

	weatherTex = 0;
	perlinTex = 0;
	worley32 = 0;
}

void CloudsModel::draw(const Camera& camera, const Sky& sky, const unsigned int sceneDepth) {

	

	glBindImageTexture(0, m_textureSet[0].getTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(1, m_textureSet[1].getTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(2, m_textureSet[2].getTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
	glBindImageTexture(3, m_textureSet[3].getTexture(), 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);

	m_raymarcher->use();
	m_raymarcher->loadVector("iResolution", Vector2f(static_cast<float>(m_width), static_cast<float>(m_height)));
	m_raymarcher->loadFloat("iTime", m_clock.getElapsedTimeSec());
	m_raymarcher->loadMatrix("inv_proj", camera.getInvPerspectiveMatrix());
	m_raymarcher->loadMatrix("inv_view", camera.getInvViewMatrix());
	m_raymarcher->loadVector("cameraPosition", camera.getPosition());
	m_raymarcher->loadFloat("FOV", 60.0f);
	m_raymarcher->loadVector("lightDirection", light.direction);
	m_raymarcher->loadVector("lightColor", light.color);

	m_raymarcher->loadFloat("coverage_multiplier", coverage);
	m_raymarcher->loadFloat("cloudSpeed", cloudSpeed);
	m_raymarcher->loadFloat("crispiness", crispiness);
	m_raymarcher->loadFloat("curliness", curliness);
	m_raymarcher->loadFloat("absorption", absorption * 0.01f);
	m_raymarcher->loadFloat("densityFactor",density);

	m_raymarcher->loadFloat("earthRadius", earthRadius);
	m_raymarcher->loadFloat("sphereInnerRadius", sphereInnerRadius);
	m_raymarcher->loadFloat("sphereOuterRadius", sphereOuterRadius);

	m_raymarcher->loadVector("cloudColorTop", cloudColorTop);
	m_raymarcher->loadVector("cloudColorBottom", cloudColorBottom);

	m_raymarcher->loadVector("skyColorTop", sky.m_skyColorTop);
	m_raymarcher->loadVector("skyColorBottom", sky.m_skyColorBottom);

	m_raymarcher->loadMatrix("invViewProj", camera.getInvViewMatrix() * camera.getInvPerspectiveMatrix());
	m_raymarcher->loadMatrix("gVP", camera.getPerspectiveMatrix() * camera.getViewMatrix());

	m_raymarcher->loadInt("cloud", 0);
	m_raymarcher->loadInt("worley32", 1);
	m_raymarcher->loadInt("weatherTex", 2);
	m_raymarcher->loadInt("depthMap", 3);
	m_raymarcher->loadInt("sky", 4);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_3D, perlinTex);

	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_3D, worley32);

	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, weatherTex);

	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, sceneDepth);

	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, sky.getSkyTexture());


	glDispatchCompute(INT_CEIL(m_width, 16), INT_CEIL(m_height, 16), 1);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

	if (postProcess) {

		postBuffer.bind();

		m_post->use();

		m_post->loadInt("clouds", 0);
		m_post->loadInt("emissions", 1);
		m_post->loadInt("depthMap", 2);

		m_textureSet[cloudsTextureNames::fragColor].bind(0);
		m_textureSet[cloudsTextureNames::bloom].bind(1);

		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, sceneDepth);

		m_post->loadVector("cloudRenderResolution", Vector2f(static_cast<float>(m_width), static_cast<float>(m_height)));
		m_post->loadVector("resolution", Vector2f(static_cast<float>(m_width), static_cast<float>(m_height)));

		Matrix4f vp = camera.getPerspectiveMatrix() * camera.getViewMatrix();
		Matrix4f lightModel;
		lightModel.translate(light.position);
		Vector4f pos = Vector4f(0.0f, 60.0f, 0.0f, 1.0f) ^ (vp * lightModel);
		pos = pos / pos[3];
		pos = pos * 0.5f + 0.5f;

		m_post->loadVector("lightPos", pos);

		bool isLightInFront = false;
		float lightDotCameraFront = Vector3f::Dot(Vector3f::Normalize(light.position - camera.getPosition()), camera.getViewDirection());
		if (lightDotCameraFront > 0.2) {
			isLightInFront = true;
		}
		m_post->loadBool("isLightInFront", isLightInFront);
		m_post->loadBool("enableGodRays", enableGodRays);
		m_post->loadFloat("lightDotCameraFront", lightDotCameraFront);
		m_post->loadFloat("time", m_clock.getElapsedTimeSec());

		m_quad.drawRaw();

		postBuffer.unbind();
	}
}

void CloudsModel::resize(unsigned int width, unsigned int height) {
	m_width = width;
	m_height = height;
}

unsigned int CloudsModel::getPostTexture() {
	return postBuffer.getColorTexture();
}

const Texture& CloudsModel::getColorTexture(cloudsTextureNames name) {
	return m_textureSet[name];
}