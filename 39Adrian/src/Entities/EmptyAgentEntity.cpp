#include <States/NavigationState.h>
#include <States/NavigationStreamState.h>
#include "EmptyAgentEntity.h"

EmptyAgentEntity::EmptyAgentEntity(const CrowdAgent& crowdAgent, SceneNodeLC* controlledNode) : CrowdAgentObject(crowdAgent, controlledNode), Entity() {

}

EmptyAgentEntity::~EmptyAgentEntity() {

}

void EmptyAgentEntity::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {

}

void EmptyAgentEntity::OnInactive() {

}

void EmptyAgentEntity::OnTarget(const Vector3f& targetPos) {
	NavigationStreamState::AddMarker(targetPos);
}

void EmptyAgentEntity::OnAddAgent(const Vector3f& targetPos) {

}