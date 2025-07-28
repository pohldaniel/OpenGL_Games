#pragma once
#include <engine/MeshObject/Shape.h>
#include <engine/Md2Model.h>

class Scene {

	friend class SceneManager;

public:

	Scene();
	~Scene();
	void loadScene();
	void loadBuilding(const char* filename, bool changeWinding = false);
	void loadMiniMap(const char* filename);

	Md2Model m_heroModel, m_hueteotl, m_mutantman, m_corpse, m_mutantlizard, m_mutantcheetah, m_ripper;
	std::vector<Shape> m_buildings;
	Shape m_segment, m_disk, m_sphere;
	std::vector<std::array<float, 4>> m_buildingsMiniMap;

	float m_xconvfactor;
	float m_yconvfactor;
};

class SceneManager {

public:

private:

	SceneManager();
	static SceneManager s_instance;
};
