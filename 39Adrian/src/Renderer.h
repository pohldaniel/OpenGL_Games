#pragma once

#include <engine/Vector.h>
#include <engine/octree/Octree.h>
#include <engine/scene/ShapeNode.h>
#include <engine/scene/SceneNodeLC.h>

class Renderer {

public:
	void addMarker(const Vector3f& pos, float scale = 1.0f, int textureIndex = 4);
	void clearMarker();
	void init(Octree* octree, SceneNodeLC* scene);
	void shutdown();
	SceneNodeLC* getScene();
	Octree* getOctree();

	static Renderer& Get();

private:

	Renderer();

	std::vector<ShapeNode*> m_marker;
	Octree* m_octree;
	SceneNodeLC* m_scene;

	static Renderer Instance;

};