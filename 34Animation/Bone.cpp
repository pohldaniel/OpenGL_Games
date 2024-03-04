#include <iostream>
#include "Bone.h"
#include <engine/scene/ShapeNode.h>

Bone::Bone() : animationEnabled(true), numChildBones(0), m_rootBone(false){

}

Bone::~Bone(){

}

void Bone::SetAnimationEnabled(bool enable){
	animationEnabled = enable;
}

void Bone::CountChildBones(){
	numChildBones = 0;

	for (auto it = m_children.begin(); it != m_children.end(); ++it){
		if (dynamic_cast<Bone*>((*it).get()))
			++numChildBones;
	}
}

void Bone::OnTransformChanged(){
	SceneNodeLC::OnTransformChanged();
}

Bone* Bone::FindChildOfType(StringHash childNameHash, bool recursive) const{
	for (auto it = m_children.begin(); it != m_children.end(); ++it){
		Bone* child = dynamic_cast<Bone*>((*it).get());
		if (!child) {
			continue;
		}

		if (child->nameHash == childNameHash)
			return child;
		else if (recursive && child->m_children.size()){
			Bone* childResult = child->FindChildOfType(childNameHash, recursive);
			if (childResult)
				return childResult;
		}
	}

	return nullptr;
}

void Bone::setRootBone(bool rootBone) {
	m_rootBone = rootBone;
}

const bool Bone::isRootBone() const {
	return m_rootBone;
}