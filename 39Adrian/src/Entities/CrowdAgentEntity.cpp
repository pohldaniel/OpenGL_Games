#include <States/NavigationState.h>
#include <States/NavigationStreamState.h>
#include "CrowdAgentEntity.h"

CrowdAgentEntity::CrowdAgentEntity(const CrowdAgent& crowdAgent, SceneNodeLC* controlledNode) : CrowdAgentObject(crowdAgent, controlledNode), Entity(){

}

CrowdAgentEntity::~CrowdAgentEntity() {

}

void CrowdAgentEntity::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {
	setOrientation(vel);
	setPosition(pos);
}

void CrowdAgentEntity::OnInactive() {

}

void CrowdAgentEntity::OnTarget(const Vector3f& targetPos) {
	NavigationStreamState::AddMarker(targetPos);
	getCrowdAgent().setForceArrived(false);
}

void CrowdAgentEntity::OnAddAgent(const Vector3f& pos) {
	setPosition(pos);
}

void CrowdAgentEntity::resetAgent() {
	getCrowdAgent().resetAgent();
}