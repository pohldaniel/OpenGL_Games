#include "OffMeshConnection.h"

static const float DEFAULT_RADIUS = 1.0f;
static const unsigned DEFAULT_MASK_FLAG = 1;
static const unsigned DEFAULT_AREA = 1;

OffMeshConnection::OffMeshConnection(SceneNodeLC* node) :
	endPointID_(0),
	radius_(DEFAULT_RADIUS),
	bidirectional_(true),
	endPointDirty_(false),
	mask_(DEFAULT_MASK_FLAG),
	areaId_(DEFAULT_AREA),
	m_node(node),
	isEnabled_(true){
}

OffMeshConnection::~OffMeshConnection(){
}

void OffMeshConnection::OnRenderDebug(){
	if (endPoint_ == nullptr)
		return;

	Vector3f start = m_node->getWorldPosition();
	Vector3f end = endPoint_->getWorldPosition();

	DebugRenderer::Get().AddSphere(start, radius_, Vector4f::ONE);
	DebugRenderer::Get().AddSphere(end, radius_, Vector4f::ONE);
	DebugRenderer::Get().AddLine(start, end, Vector4f::ONE);
}

void OffMeshConnection::SetRadius(float radius){
	radius_ = radius;
}

void OffMeshConnection::SetBidirectional(bool enabled){
	bidirectional_ = enabled;
}

void OffMeshConnection::SetMask(unsigned newMask){
	mask_ = newMask;
}

void OffMeshConnection::SetAreaID(unsigned newAreaID){
	areaId_ = newAreaID;
}

void OffMeshConnection::SetEndPoint(SceneNodeLC* node){
	endPoint_ = node;
}

SceneNodeLC* OffMeshConnection::GetEndPoint() const{
	return endPoint_;
}