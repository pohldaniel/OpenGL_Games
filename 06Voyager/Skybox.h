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

	SkyBox(const float scale, const Vector3f& position = Vector3f(0.0f, 0.0f, 0.0f));
	~SkyBox();

	void render(const Camera& camera);
	void toggleDayNight();
private:
	void createBuffer();

	float m_scale = 50.0f;
	Vector3f m_position = Vector3f(0.0f, 0.0f, 0.0f);

	std::vector<float> m_vertices;
	unsigned int m_vbo, m_vao;

	Shader* m_skyboxShader;
	Cubemap* m_cubemap;

	Matrix4f m_model;

	bool m_day = true;
};