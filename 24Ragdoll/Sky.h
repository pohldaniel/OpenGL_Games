#pragma once
#include "engine/MeshObject/MeshQuad.h"
#include "engine/Vector.h"
#include "engine/Camera.h"
#include "engine/Texture.h"
#include "engine/Shader.h"
#include "engine/Framebuffer.h"
#include "Light.h"

struct colorPreset {
	Vector3f cloudColorBottom, skyColorTop, skyColorBottom, lightColor, fogColor;
};

class Sky {

	friend class CloudsModel;

public:

	Sky(unsigned int width, unsigned int height, Light& light);
	~Sky();

	void draw(const Camera& camera);
	void draw(const Matrix4f& invProj, const Matrix4f& invView);
	void update();
	void resize(unsigned int width, unsigned int height);
	const Vector3f& getFogColor();
	const unsigned int& getSkyTexture() const;

private:

	colorPreset DefaultPreset();
	colorPreset SunsetPreset();
	colorPreset SunsetPreset1();
	void mixSkyColorPreset(float v, colorPreset p1, colorPreset p2);

	colorPreset presetSunset, highSunPreset;
	Framebuffer m_skyBuffer;
	Vector3f m_skyColorTop, m_skyColorBottom, m_fogColor;
	unsigned int width, height;

	Light& light;
	Shader *m_shader;
	MeshQuad m_quad;
};



