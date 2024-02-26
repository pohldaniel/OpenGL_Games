#include <iostream>
#include "Bone.h"

Bone::Bone() : animationEnabled(true), numChildBones(0), m_rootBone(false)
	//,drawable(nullptr)
{
}

Bone::~Bone(){

}


void Bone::SetAnimationEnabled(bool enable){
	animationEnabled = enable;
}

void Bone::CountChildBones(){
	numChildBones = 0;

	for (auto it = m_children.begin(); it != m_children.end(); ++it){
		//if ((*it)->Type() == Bone::TypeStatic())
			++numChildBones;
	}
}

void Bone::OnTransformChanged(){
	SceneNodeLC::OnTransformChanged();

	// Avoid duplicate dirtying calls if model's skinning is already dirty. Do not signal changes either during animation update,
	// as the model will set the hierarchy dirty when finished. This is also used to optimize when only the model node moves.
	//if (drawable && !(drawable->AnimatedModelFlags() & (AMF_IN_ANIMATION_UPDATE | AMF_SKINNING_DIRTY)))
	//	drawable->OnBoneTransformChanged();
}

Bone* Bone::FindChildOfType(StringHash childNameHash, bool recursive) const{
	
	
	//std::list<std::unique_ptr<BaseNode>>::iterator it = std::find_if(m_children.begin(), m_children.end(), [childNameHash](std::unique_ptr<BaseNode>& _node) { return dynamic_cast<Bone*>(_node.get())->nameHash == childNameHash; });
	//if (it != m_children.end())
		//return dynamic_cast<Bone*>((*it).get());
	//else
		//return nullptr;

	for (auto it = m_children.begin(); it != m_children.end(); ++it){
		Bone* child = dynamic_cast<Bone*>((*it).get());
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