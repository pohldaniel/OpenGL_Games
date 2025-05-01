#include "Obstacle.h"
#include "DynamicNavigationMesh.h"
#include "../engine/DebugRenderer.h"

Obstacle::Obstacle(OctreeNode* node) :
	m_node(node),
	m_height(5.0f),
	m_radius(5.0f),
	m_obstacleId(0),
	m_ownerMesh(nullptr),
	m_isEnabled(true){
}

Obstacle::~Obstacle(){
	if (m_obstacleId > 0 && m_ownerMesh) {
		m_ownerMesh->removeObstacle(this);
	}
	m_ownerMesh = nullptr;
	m_node = nullptr;
}

void Obstacle::setOwnerMesh(DynamicNavigationMesh* ownerMesh) {
	m_ownerMesh = ownerMesh;
}

void Obstacle::setIsEnabled(bool isEnabled) {
	m_isEnabled = isEnabled;
}

void Obstacle::setNode(OctreeNode* node) {
	m_node = node;
}

void Obstacle::OnSetEnabled() {
	if (m_ownerMesh){
		if (m_isEnabled)
			m_ownerMesh->addObstacle(this);
		else
			m_ownerMesh->removeObstacle(this);
	}
}

void Obstacle::setHeight(float newHeight) {
	m_height = newHeight;
	if (m_ownerMesh)
		m_ownerMesh->obstacleChanged(this);

}

void Obstacle::setRadius(float newRadius) {
	m_radius = newRadius;
	if (m_ownerMesh)
		m_ownerMesh->obstacleChanged(this);
}


void Obstacle::OnRenderDebug() {
	if(m_isEnabled)
	  DebugRenderer::Get().AddCylinder(m_node->getWorldPosition(), m_radius, m_height, Vector4f(0.0f, 1.0f, 1.0f, 1.0f));
}

void Obstacle::OnTileAdded(const std::array<int, 2>& tile) {
	if (m_isEnabled && m_ownerMesh && m_ownerMesh->isObstacleInTile(this, tile))
		m_ownerMesh->obstacleChanged(this);
}

OctreeNode* Obstacle::getNode() {
	return m_node;
}

float Obstacle::getHeight() const {
	return m_height;
}

float Obstacle::getRadius() const {
	return m_radius;
}

unsigned Obstacle::getObstacleID() const { 
	return m_obstacleId;
}