#include <States/NavigationState.h>
#include <States/NavigationStreamState.h>
#include "EmptyAgentEntity.h"
#include "Renderer.h"

EmptyAgentEntity::EmptyAgentEntity(const CrowdAgent& crowdAgent) : CrowdAgentObject(crowdAgent, nullptr) {

}

EmptyAgentEntity::~EmptyAgentEntity() {

}

void EmptyAgentEntity::OnPositionVelocityUpdate(const Vector3f& pos, const Vector3f& vel) {

}

void EmptyAgentEntity::OnInactive() {

}

void EmptyAgentEntity::OnTarget(const Vector3f& targetPos) {

}

void EmptyAgentEntity::OnAddAgent(const Vector3f& pos) {

}

void EmptyAgentEntity::resetAgent() {
	getCrowdAgent().resetAgent();
}