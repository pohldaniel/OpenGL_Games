#include "Obstacle.h"
#include "DynamicNavigationMesh.h"


Obstacle::Obstacle(OctreeNode* node) :
	m_node(node),
	height_(5.0f),
	radius_(5.0f),
	obstacleId_(0),
	ownerMesh_(nullptr){

}

Obstacle::~Obstacle(){
	if (obstacleId_ > 0 && ownerMesh_) {
		ownerMesh_->RemoveObstacle(this);
	}
	ownerMesh_ = nullptr;
	m_node = nullptr;
}

void Obstacle::OnSetEnabled() {
	if (ownerMesh_){
		if (isEnabled_)
			ownerMesh_->AddObstacle(this);
		else
			ownerMesh_->RemoveObstacle(this);
	}
}

void Obstacle::SetHeight(float newHeight) {
	height_ = newHeight;
	//if (ownerMesh_)
		//ownerMesh_->ObstacleChanged(this);

}

void Obstacle::SetRadius(float newRadius) {
	radius_ = newRadius;
	//if (ownerMesh_)
		//ownerMesh_->ObstacleChanged(this);
}

void Obstacle::OnNodeSet(OctreeNode* node) {
	//if (node)
		//node->AddListener(this);
}

/*void Obstacle::OnSceneSet(Scene* scene)
{
	if (scene)
	{
		if (scene == node_)
		{
			URHO3D_LOGWARNING(GetTypeName() + " should not be created to the root scene node");
			return;
		}
		if (!ownerMesh_)
			ownerMesh_ = node_->GetParentComponent<DynamicNavigationMesh>(true);
		if (ownerMesh_)
		{
			ownerMesh_->AddObstacle(this);
			SubscribeToEvent(ownerMesh_, E_NAVIGATION_TILE_ADDED, URHO3D_HANDLER(Obstacle, HandleNavigationTileAdded));
		}
	}
	else
	{
		if (obstacleId_ > 0 && ownerMesh_)
			ownerMesh_->RemoveObstacle(this);

		UnsubscribeFromEvent(E_NAVIGATION_TILE_ADDED);
		ownerMesh_.Reset();
	}
}*/

void Obstacle::OnMarkedDirty(OctreeNode* node) {
	if (isEnabled_ && ownerMesh_) {
		/*Scene* scene = GetScene();
		/// \hack If scene already unassigned, or if it's being destroyed, do nothing
		if (!scene || scene->Refs() == 0)
			return;

		// If within threaded update, update later
		if (scene->IsThreadedUpdate())
		{
			scene->DelayedMarkedDirty(this);
			return;
		}

		ownerMesh_->ObstacleChanged(this);*/
	}
}

/*void Obstacle::HandleNavigationTileAdded(StringHash eventType, VariantMap& eventData)
{
	// Re-add obstacle if it is intersected with newly added tile
	const IntVector2 tile = eventData[NavigationTileAdded::P_TILE].GetIntVector2();
	if (IsEnabledEffective() && ownerMesh_ && ownerMesh_->IsObstacleInTile(this, tile))
		ownerMesh_->ObstacleChanged(this);
}*/

void Obstacle::OnRenderDebug() {
	if(isEnabled_)
	  DebugRenderer::Get().AddCylinder(m_node->getWorldPosition(), radius_, height_, Vector4f(0.0f, 1.0f, 1.0f, 1.0f));
}

void Obstacle::OnTileAdded(const std::array<int, 2>& tile) {
	if (isEnabled_ && ownerMesh_ && ownerMesh_->IsObstacleInTile(this, tile))
		ownerMesh_->ObstacleChanged(this);
}