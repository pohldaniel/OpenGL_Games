#include "OffMeshConnection.h"

static const float DEFAULT_RADIUS = 1.0f;
static const unsigned DEFAULT_MASK_FLAG = 1;
static const unsigned DEFAULT_AREA = 1;

OffMeshConnection::OffMeshConnection(SceneNodeLC* node) :
	m_radius(DEFAULT_RADIUS),
	m_bidirectional(true),
	m_endPointDirty(false),
	m_mask(DEFAULT_MASK_FLAG),
	m_areaId(DEFAULT_AREA),
	m_node(node),
	m_endPoint(nullptr),
	m_isEnabled(true){
}

OffMeshConnection::OffMeshConnection(SceneNodeLC* node, SceneNodeLC* endPoint):
	m_radius(DEFAULT_RADIUS),
	m_bidirectional(true),
	m_endPointDirty(false),
	m_mask(DEFAULT_MASK_FLAG),
	m_areaId(DEFAULT_AREA),
	m_node(node),
	m_endPoint(endPoint),
	m_isEnabled(true) {

}

OffMeshConnection::OffMeshConnection(OffMeshConnection const& rhs) :
	m_node(rhs.m_node),
	m_endPoint(rhs.m_endPoint),
	m_radius(rhs.m_radius),
	m_endPointDirty(rhs.m_endPointDirty),
	m_mask(rhs.m_mask),
	m_areaId(rhs.m_areaId),
	m_bidirectional(rhs.m_bidirectional),
	m_isEnabled(rhs.m_isEnabled)

{

}

OffMeshConnection::OffMeshConnection(OffMeshConnection&& rhs) :
	m_node(rhs.m_node),
	m_endPoint(rhs.m_endPoint),
	m_radius(rhs.m_radius),
	m_endPointDirty(rhs.m_endPointDirty),
	m_mask(rhs.m_mask),
	m_areaId(rhs.m_areaId),
	m_bidirectional(rhs.m_bidirectional),
	m_isEnabled(rhs.m_isEnabled) {

}

OffMeshConnection& OffMeshConnection::operator=(const OffMeshConnection& rhs) {
	m_node = rhs.m_node;
	m_endPoint = rhs.m_endPoint;
	m_radius = rhs.m_radius;
	m_endPointDirty = rhs.m_endPointDirty;
	m_mask = rhs.m_mask;
	m_areaId = rhs.m_areaId;
	m_bidirectional = rhs.m_bidirectional;
	m_isEnabled = rhs.m_isEnabled;
	return *this;
}

OffMeshConnection& OffMeshConnection::operator=(OffMeshConnection&& rhs) {
	m_node = rhs.m_node;
	m_endPoint = rhs.m_endPoint;
	m_radius = rhs.m_radius;
	m_endPointDirty = rhs.m_endPointDirty;
	m_mask = rhs.m_mask;
	m_areaId = rhs.m_areaId;
	m_bidirectional = rhs.m_bidirectional;
	m_isEnabled = rhs.m_isEnabled;
	return *this;
}

OffMeshConnection::~OffMeshConnection(){

}

void OffMeshConnection::OnRenderDebug() const {
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

void OffMeshConnection::setMask(unsigned int newMask){
	m_mask = newMask;
}

void OffMeshConnection::setAreaID(unsigned int newAreaID){
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