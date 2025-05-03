#include "Renderer.h"
#include "Globals.h"

Renderer Renderer::Instance;

Renderer::Renderer() : m_octree(nullptr), m_scene(nullptr) {

}

void Renderer::init(Octree* octree, SceneNodeLC* scene) {
	m_octree = octree;
	m_scene = scene;
}

void Renderer::shutdown() {
	clearMarker();

	delete m_octree;
	m_octree = nullptr;

	delete m_scene;
	m_scene = nullptr;
}

SceneNodeLC* Renderer::getScene() {
	return m_scene;
}

Octree* Renderer::getOctree() {
	return m_octree;
}

void Renderer::addMarker(const Vector3f& pos, float scale, int textureIndex) {
	m_marker.push_back(m_scene->addChild<ShapeNode, Shape>(Globals::shapeManager.get("sphere"), true));
	m_marker.back()->setPosition(pos);
	m_marker.back()->setScale(scale);
	m_marker.back()->setTextureIndex(textureIndex);
	m_marker.back()->OnOctreeSet(m_octree);
}

void Renderer::clearMarker() {
	for (auto shapeNode : m_marker) {
		shapeNode->OnOctreeSet(nullptr);
		shapeNode->eraseSelf();
	}
	m_marker.clear();
	m_marker.shrink_to_fit();
}

Renderer& Renderer::Get() {
	return Instance;
}