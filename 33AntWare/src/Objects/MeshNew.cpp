#include "MeshNew.h"

SceneNode* MeshNew::getOwner() const { 
	return owner; 
}

void MeshNew::setOwner(SceneNode* _owner){
	owner = _owner;
	worldTransform = const_cast<Matrix4f*>(&owner->getWorldTransformation());
}

const Vector3f& MeshNew::getPosition() const {
	return owner->getPosition(); 
}

const Quaternion& MeshNew::getdOrientation() const { 
	return  owner->getOrientation(); 
}

const Vector3f& MeshNew::getScale() const { 
	return owner->getScale(); 
}

const BoundingBox& MeshNew::getLocalBoundingBox() const {
	return localBoundingBox;
}

void MeshNew::setLocalBoundingBox(const BoundingBox& box) {
	localBoundingBox = box;
}