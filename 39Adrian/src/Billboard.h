#pragma once

#include <vector>
#include <engine/Vector.h>
#include <engine/Camera.h>

class Billboard {

public:

	Billboard(const IsometricCamera& camera);
	~Billboard();
	void draw();
	void drawShadow(const Matrix4f& shadowMatrix, const Vector3f& lightPos, const Vector3f& lightRight);

private:

	void loadBillboards();

	const IsometricCamera& camera;
	std::vector<Vector3f> m_positions;
	unsigned int m_vao;
	unsigned int m_vbo;
};