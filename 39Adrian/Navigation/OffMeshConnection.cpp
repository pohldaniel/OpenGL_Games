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
	m_node(node){
}

OffMeshConnection::~OffMeshConnection(){
}

void OffMeshConnection::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
	if (endPoint_ == nullptr)
		return;

	//Vector3f start = node_->GetWorldPosition();
	Vector3f start = Vector3f(0.0f, 0.0f, 0.0f);
	Vector3f end = endPoint_->getWorldPosition();

	debug->AddSphere(start, radius_, Vector4f::ONE);
	debug->AddSphere(end, radius_, Vector4f::ONE);
	debug->AddLine(start, end, Vector4f::ONE);
}

void OffMeshConnection::SetRadius(float radius)
{
	radius_ = radius;
	//MarkNetworkUpdate();
}

void OffMeshConnection::SetBidirectional(bool enabled)
{
	bidirectional_ = enabled;
	//MarkNetworkUpdate();
}

void OffMeshConnection::SetMask(unsigned newMask)
{
	mask_ = newMask;
	//MarkNetworkUpdate();
}

void OffMeshConnection::SetAreaID(unsigned newAreaID)
{
	areaId_ = newAreaID;
	//MarkNetworkUpdate();
}

void OffMeshConnection::SetEndPoint(SceneNodeLC* node)
{
	endPoint_ = node;
	//endPointID_ = node ? node->GetID() : 0;
	//MarkNetworkUpdate();
}

SceneNodeLC* OffMeshConnection::GetEndPoint() const
{
	return endPoint_;
}