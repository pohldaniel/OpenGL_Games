#include "MeshNew.h"


SceneNode*  MeshNew::getOwner() const { 
	return owner; 
}

void MeshNew::setOwner(SceneNode* _owner){
	owner = _owner;
	worldTransform = const_cast<Matrix4f*>(&owner->getTransformation());
}