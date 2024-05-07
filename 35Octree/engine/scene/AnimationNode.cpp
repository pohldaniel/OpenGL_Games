#include "AnimationNode.h"
#include "../DebugRenderer.h"

AnimationNode::AnimationNode(const AnimatedModel& animatedModel) : OctreeNode(animatedModel.getAABB()), animatedModel(animatedModel) {
	OnBoundingBoxChanged();
}

void AnimationNode::addChild(AnimationNode* node, bool drawDebug) {
	SceneNodeLC::addChild(node);
}

const AnimatedModel& AnimationNode::getAnimatedModel() const {
	return animatedModel;
}

void AnimationNode::OnBoundingBoxChanged() {
	OctreeNode::OnBoundingBoxChanged();
	m_boneBoundingBoxDirty = true;
}

void AnimationNode::OnWorldBoundingBoxUpdate() const {
	if (animatedModel.m_meshes[0]->m_meshBones.size()){
		// Recalculate bounding box from bones only if they moved individually
		if (m_boneBoundingBoxDirty){
			const std::vector<MeshBone>& meshBones = animatedModel.m_meshes[0]->m_meshBones;

			// Use a temporary bounding box for calculations in case many threads call this simultaneously
			BoundingBox tempBox;

			for (size_t i = 0; i < animatedModel.m_meshes[0]->m_meshBones.size(); ++i){
				if (meshBones[i].active)
					tempBox.merge(meshBones[i].boundingBox.transformed(animatedModel.m_meshes[0]->m_bones[i]->getWorldTransformation()));
			}

			m_worldBoundingBox = tempBox;
			m_boneBoundingBox = tempBox.transformed(Matrix4f::Inverse(getWorldTransformation()));
			m_boneBoundingBoxDirty = false;
		}else {
			m_worldBoundingBox = m_boneBoundingBox.transformed(getWorldTransformation());
		}
	}else {
		OctreeNode::OnWorldBoundingBoxUpdate();
	}
}

void AnimationNode::OnOctreeUpdate() {
	OctreeNode::OnOctreeUpdate();
	/*if (m_animationDirty) {
		animatedModel.update(dt);
		m_animationDirty = false;
	}

	if (m_skinningDirty) {
		animatedModel.updateSkinning();
		m_skinningDirty = false;
	}*/
}