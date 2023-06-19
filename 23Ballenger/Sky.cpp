#include <GL/glew.h>
#include "Sky.h"
#include "Globals.h"

Sky::Sky(unsigned int width, unsigned int height, Light& light) : width(width), height(height), light(light) {
	m_skyBuffer.create(width, height); 
	m_skyBuffer.attachTexture2D(AttachmentTex::RGBA);

	SunsetPreset1();
	DefaultPreset();
	update();

	m_shader = new Shader("res/clouds/sky.vert", "res/clouds/sky.frag");
	m_quad.create(Vector3f(-1.0f, -1.0f, 0.0f), Vector2f(2.0f, 2.0f), true, false, false, 1, 1);
}

Sky::~Sky() {
	delete m_shader;
}

void Sky::draw(const Camera& camera) {
	draw(camera.getInvPerspectiveMatrixNew(), camera.getInvViewMatrix());
}

void Sky::draw(const Matrix4f& invProj, const Matrix4f& invView) {

	m_skyBuffer.bind();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	m_shader->use();
	m_shader->loadVector("resolution", Vector2f(static_cast<float>(width), static_cast<float>(height)));
	m_shader->loadMatrix("inv_proj", invProj);
	m_shader->loadMatrix("inv_view", invView);
	m_shader->loadVector("lightDirection", light.direction);
	m_shader->loadVector("skyColorTop", m_skyColorTop);
	m_shader->loadVector("skyColorBottom", m_skyColorBottom);
	m_quad.drawRaw();

	m_shader->unuse();
	m_skyBuffer.unbind();
}

void Sky::update() {
	auto sigmoid = [](float v) { return 1.0f / (1.0f + expf(8.0f - v * 40.0f)); };
	mixSkyColorPreset(sigmoid(light.direction[1]), highSunPreset, presetSunset);
}

void Sky::resize(unsigned int _width, unsigned int _height) {
	width = _width;
	height = _height;
}

colorPreset Sky::SunsetPreset() {
	colorPreset preset;

	preset.cloudColorBottom = Vector3f(89.0f, 96.0f, 109.0f) / 255.0f;
	preset.skyColorTop = Vector3f(177.0f, 174.0f, 119.0f) / 255.0f;
	preset.skyColorBottom = Vector3f(234.0f, 125.0f, 125.0f) / 255.0f;

	preset.lightColor = Vector3f(255.0f, 171.0f, 125.0f) / 255.0f;
	preset.fogColor = Vector3f(85.0f, 97.0f, 120.0f) / 255.0f;

	presetSunset = preset;

	return preset;
}

colorPreset Sky::SunsetPreset1() {
	colorPreset preset;

	preset.cloudColorBottom = Vector3f(97.0f, 98.0f, 120.0f) / 255.0f;
	preset.skyColorTop = Vector3f(133.0f, 158.0f, 214.0f) / 255.0f;
	preset.skyColorBottom = Vector3f(241.0f, 161.0f, 161.0f) / 255.0f;

	preset.lightColor = Vector3f(255.0f, 201.0f, 201.0f) / 255.0f;
	preset.fogColor = Vector3f(128.0f, 153.0f, 179.0f) / 255.0f;

	presetSunset = preset;

	return preset;
}

colorPreset Sky::DefaultPreset() {
	colorPreset preset;

	preset.cloudColorBottom = (Vector3f(65.0f, 70.0f, 80.0f) * (1.5f / 255.f));

	preset.skyColorTop = Vector3f(0.5f, 0.7f, 0.8f) * 1.05f;
	preset.skyColorBottom = Vector3f(0.9f, 0.9f, 0.95f);

	preset.lightColor = Vector3f(255.0f, 255.0f, 230.0f) / 255.0f;
	preset.fogColor = Vector3f(0.5f, 0.6f, 0.7f);

	highSunPreset = preset;

	return preset;
}

void Sky::mixSkyColorPreset(float v, colorPreset p1, colorPreset p2) {
	float a = std::min(std::max(v, 0.0f), 1.0f);
	float b = 1.0f - a;

	m_skyColorTop = p1.skyColorTop * a + p2.skyColorTop * b;
	m_skyColorBottom = p1.skyColorBottom * a + p2.skyColorBottom * b;
	light.color = p1.lightColor * a + p2.lightColor * b;
	m_fogColor = p1.fogColor * a + p2.fogColor * b;
}

const Vector3f& Sky::getFogColor() {
	return m_fogColor;
}

const unsigned int& Sky::getSkyTexture() const{
	return m_skyBuffer.getColorTexture();
}