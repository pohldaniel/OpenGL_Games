#include <States/NavigationState.h>
#include "CrowdAgentEntity.h"

CrowdAgentEntity::CrowdAgentEntity(const CrowdAgent& crowdAgent, SceneNodeLC* controlledNode) : CrowdAgentObject(crowdAgent, controlledNode), Entity(){

}

void CrowdAgentEntity::draw() {

}

void CrowdAgentEntity::update(const float dt) {

}

void CrowdAgentEntity::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {
	setOrientation(-vel);
	setPosition(pos);
}

void CrowdAgentEntity::OnInactive() {

}

void CrowdAgentEntity::OnTarget(const Vector3f& targetPos) {
	NavigationState::AddMarker(targetPos);
}