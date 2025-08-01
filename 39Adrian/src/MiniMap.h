#pragma once
#include <engine/Camera.h>
#include "Scene.h"

class MiniMap {

public:

	MiniMap(const IsometricCamera& camera, const Scene& scene, const std::vector<Md2Entity*>& entities);
	~MiniMap();

	void draw();
	void init();
	bool isMouseOver(int sx, int sy, float &, float &);

private:

	void updateEntitiePositions();

	const IsometricCamera& camera;
	const Scene& scene;
	const std::vector<Md2Entity*>& entities;

	unsigned int m_vao;
	unsigned int m_vbo;
	std::vector<std::array<float, 6>> m_entitiesPosCol;
};