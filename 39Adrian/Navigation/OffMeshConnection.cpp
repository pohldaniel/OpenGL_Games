#include "OffMeshConnection.h"

static const float DEFAULT_RADIUS = 1.0f;
static const unsigned DEFAULT_MASK_FLAG = 1;
static const unsigned DEFAULT_AREA = 1;

OffMeshConnection::OffMeshConnection(SceneNodeLC* node) :
	m_endPointID(0u),
	m_radius(DEFAULT_RADIUS),
	m_bidirectional(true),
	m_endPointDirty(false),
	m_mask(DEFAULT_MASK_FLAG),
	m_areaId(DEFAULT_AREA),
	m_node(node),
	m_isEnabled(true){
}

OffMeshConnection::~OffMeshConnection(){

}

void OffMeshConnection::OnRenderDebug(){
	if (m_endPoint == nullptr)
		return;

	Vector3f start = m_node->getWorldPosition();
	Vector3f end = m_endPoint->getWorldPosition();

	DebugRenderer::Get().AddSphere(start, m_radius, Vector4f::ONE);
	DebugRenderer::Get().AddSphere(end, m_radius, Vector4f::ONE);
	DebugRenderer::Get().AddLine(start, end, Vector4f::ONE);
}

void OffMeshConnection::setRadius(float radius){
	m_radius = radius;
}

void OffMeshConnection::setBidirectional(bool enabled){
	m_bidirectional = enabled;
}

void OffMeshConnection::setMask(unsigned newMask){
	m_mask = newMask;
}

void OffMeshConnection::setAreaID(unsigned newAreaID){
	m_areaId = newAreaID;
}

void OffMeshConnection::setEndPoint(SceneNodeLC* node){
	m_endPoint = node;
}

SceneNodeLC* OffMeshConnection::getNode() const {
	return m_node;
}

SceneNodeLC* OffMeshConnection::getEndPoint() const{
	return m_endPoint;
}

float OffMeshConnection::getRadius() const { 
	return m_radius;
}

unsigned OffMeshConnection::getMask() const { 
	return m_mask;
}

unsigned OffMeshConnection::getAreaID() const { 
	return m_areaId;
}

bool OffMeshConnection::isBidirectional() const {
	return m_bidirectional;
}

bool OffMeshConnection::isEnabled() const {
	return m_isEnabled;
}