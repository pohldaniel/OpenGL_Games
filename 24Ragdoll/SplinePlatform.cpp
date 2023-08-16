#include "SplinePlatform.h"

SplinePlatform::SplinePlatform() : rotation_(0.3f) {

}

SplinePlatform::~SplinePlatform() {

}

void SplinePlatform::Initialize(SplinePath* splinePath, btRigidBody* body){
	// get other lift components
	splinePath_ = splinePath;
	rigidBody_ = body;
	controlNode_ = splinePath->GetControlledNode();
}

btRigidBody* SplinePlatform::getRigidBody() {
	return rigidBody_;
}

void SplinePlatform::FixedUpdate(float timeStep){

	if (splinePath_){
		splinePath_->Move(timeStep);

		// looped path, reset to continue
		if (splinePath_->IsFinished()){
			splinePath_->Reset();
		}

		// rotate
		if (controlNode_){
			QuaternionTu drot(rotation_, Vector3(0, 1, 0));
			QuaternionTu nrot = controlNode_->WorldRotation();
			controlNode_->SetWorldRotation(nrot * drot);

			Vector3 curPos = controlNode_->Position();

			rigidBody_->getMotionState()->setWorldTransform(Physics::BtTransform(btVector3(curPos.x, curPos.y, curPos.z)));
		}
	}
}