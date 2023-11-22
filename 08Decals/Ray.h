#pragma once

#include "engine/Vector.h"
#include "engine/Extension.h"
#include "engine/Camera.h"
#include "engine/Shader.h"

#include "Constants.h"

class Ray {

public:

	Ray();
	~Ray();
	void draw(const Camera& camera);
	void update(const Vector3f& origin, const Vector3f& direction);

private:
	void createBuffer();

	std::vector<float> m_vertices;
	unsigned int m_vbo, m_vao, m_ibo;

	Shader* m_rayShader;

	Vector3f m_origin;
	Vector3f m_direction;
};