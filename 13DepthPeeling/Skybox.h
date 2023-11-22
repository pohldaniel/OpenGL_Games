#pragma once
#include <vector>
#include "engine/Vector.h"
#include "engine/Extension.h"
#include "engine/Camera.h"
#include "engine/Shader.h"
#include "Cubemap.h"
#include "Constants.h"

class SkyBox {

public:
	
	SkyBox(const float& dt, const float& fdt, const float scale, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f));
	~SkyBox();

	void render(const Camera& camera);
	void update();
	void toggleDayNight();
private:
	void createBuffer();

	float m_scale = 50.0f;
	Vector3f m_position = Vector3f(0.0f, 0.0f, 0.0f);

	std::vector<float> m_vertices;
	unsigned int m_vbo, m_vao;

	Shader* m_skyboxShader;
	Cubemap* m_cubemapDay;
	Cubemap* m_cubemapNight;

	Matrix4f m_model;


	const float& m_fdt;
	const float& m_dt;
	const float m_rotationSpeed = 0.3f;
	float m_rotation = 0.0f;

	const float m_transitionSpeed = 0.5f;
	bool m_transitionEnd = false;	
	bool m_fadeIn = false;
	bool m_fadeOut = true;
	float m_blend = 0.0f;
};