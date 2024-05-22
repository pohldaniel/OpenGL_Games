#include "AnimationNode.h"
#include "../DebugRenderer.h"
#include "../BuiltInShader.h"
#include "../octree/Octree.h"

bool AnimationNode::CompareAnimationStates(const std::shared_ptr<AnimationState>& lhs, const std::shared_ptr<AnimationState>& rhs) {
	return lhs->getBlendLayer() < rhs->getBlendLayer();
}

AnimationNode::AnimationNode(const AnimatedModel& animatedModel) :
	OctreeNode(animatedModel.getAABB()), 
	animatedModel(animatedModel), 
	meshBones(animatedModel.m_meshes[0]->getMeshBones()),
	m_animationOrderDirty(true), 
	m_hasAnimationController(false), 
	m_animationDirty(true),
	m_skinningDirty(true),
	m_numBones(0), 
	m_updateSilent(false){
	OnBoundingBoxChanged();
	createBones();
}

AnimationNode::~AnimationNode() {
	//IMPORTANT: Will be deleted by "delete this" using the parent relationship;
	//for (size_t i = 0; i < m_numBones; ++i) {
	//	delete m_bones[i];
	//}
	delete[] m_bones;
	delete[] m_skinMatrices;
}

void AnimationNode::addChild(AnimationNode* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

const AnimatedModel& AnimationNode::getAnimatedModel() const {
	return animatedModel;
}

void AnimationNode::OnBoundingBoxChanged() {
	OctreeNode::OnBoundingBoxChanged();
	m_boneBoundingBoxDirty = true;
}

void AnimationNode::OnTransformChanged() {
	OctreeNode::OnTransformChanged();
	m_skinningDirty = true;
	m_boneBoundingBoxDirty = true;
}

void AnimationNode::OnWorldBoundingBoxUpdate() const {
	if (meshBones.size()){
		// Recalculate bounding box from bones only if they moved individually
		if (m_boneBoundingBoxDirty){
			// Use a temporary bounding box for calculations in case many threads call this simultaneously
			BoundingBox tempBox;

			for (size_t i = 0; i < meshBones.size(); ++i){
				if (meshBones[i].active)
					tempBox.merge(meshBones[i].boundingBox.transformed(m_bones[i]->getWorldTransformation()));
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

void AnimationNode::OnOctreeSet(Octree* octree) {
	OctreeNode::OnOctreeSet(octree);
	for (size_t i = 0; i < m_numBones; ++i) {
		m_skinMatrices[i] = m_bones[i]->getWorldTransformation() * meshBones[i].offsetMatrix;
	}
	m_skinningDirty = true;
}

void AnimationNode::drawRaw() const {
	glBindBuffer(GL_UNIFORM_BUFFER, BuiltInShader::matrixUbo);
	glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(Matrix4f) * m_numBones, m_skinMatrices);
	glBindBuffer(GL_UNIFORM_BUFFER, 0);

	animatedModel.drawRaw();
}

void AnimationNode::update(float dt) {
	if (frameNumber && wasInView(*frameNumber) || m_updateSilent){
		OnAnimationChanged();
		if (m_animationDirty) {
			updateAnimation(dt);
			OnBoundingBoxChanged();
			OnWorldBoundingBoxUpdate();
			m_skinningDirty = true;
		}
		if(m_skinningDirty)
			updateSkinning();
	}
}

void AnimationNode::updateSkinning() {
	for (size_t i = 0; i < m_numBones; ++i) {
		m_skinMatrices[i] = m_bones[i]->getWorldTransformation() * meshBones[i].offsetMatrix;
	}
	m_skinningDirty = false;
}

void AnimationNode::updateAnimation(float dt) {

	if (m_animationOrderDirty) {
		std::sort(m_animationStates.begin(), m_animationStates.end(), AnimationNode::CompareAnimationStates);
		m_animationOrderDirty = false;
	}

	for (size_t i = 0; i < m_numBones; ++i) {
		BoneNode* bone = m_bones[i];
		const MeshBone& meshBone = meshBones[i];
		if (bone->animationEnabled()) {
			bone->setTransformSilent(meshBone.initialPosition, meshBone.initialRotation, meshBone.initialScale);
		}
	}

	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		AnimationState* state = (*it).get();

		if (m_hasAnimationController) {
			if (state->isEnabled()) {				
				state->apply();
			}
		}else {

			if (state->isEnabled() || state->getAnimationBlendMode() == ABM_FADE) {
				state->addTime(dt);
				state->apply();
			}
		}
	}
	m_animationDirty = false;
}

AnimationState* AnimationNode::addAnimationState(Animation* animation) {

	if (!animation || !m_numBones)
		return nullptr;

	// Check for not adding twice
	AnimationState* existing = findAnimationState(animation);
	if (existing)
		return existing;

	m_animationStates.push_back(std::make_shared<AnimationState>(animation, m_rootBone));
	OnAnimationOrderChanged();
	return m_animationStates.back().get();
}

AnimationState* AnimationNode::addAnimationStateFront(Animation* animation) {

	if (!animation || !m_numBones)
		return nullptr;

	// Check for not adding twice
	AnimationState* existing = findAnimationState(animation);
	if (existing)
		return existing;

	m_animationStates.insert(m_animationStates.begin(), std::make_shared<AnimationState>(animation, m_rootBone));
	return m_animationStates.front().get();
}

AnimationState* AnimationNode::getAnimationState(size_t index) const {
	return (index < m_animationStates.size()) ? m_animationStates[index].get() : nullptr;
}

AnimationState* AnimationNode::findAnimationState(Animation* animation) const {

	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		if ((*it)->getAnimation() == animation)
			return (*it).get();
	}

	return nullptr;
}

AnimationState* AnimationNode::findAnimationState(const std::string& animationName) const {
	return getAnimationState(StringHash(animationName));
}

AnimationState* AnimationNode::findAnimationState(const char* animationName) const {
	return getAnimationState(StringHash(animationName));
}

AnimationState* AnimationNode::findAnimationState(StringHash animationNameHash) const {
	AnimationState* state = nullptr;
	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		Animation* animation = (*it)->getAnimation();
		if (animation->m_animationNameHash == animationNameHash) {
			return (*it).get();
		}

	}
	return state;
}

void AnimationNode::removeAnimationState(Animation* animation) {
	if (animation)
		removeAnimationState(animation->m_animationNameHash);
}

void AnimationNode::removeAnimationState(const std::string& animationName) {
	removeAnimationState(StringHash(animationName));
}

void AnimationNode::removeAnimationState(const char* animationName) {
	removeAnimationState(StringHash(animationName));
}

void AnimationNode::removeAnimationState(StringHash animationNameHash) {
	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		AnimationState* state = (*it).get();
		Animation* animation = state->getAnimation();

		if (animation->m_animationNameHash == animationNameHash) {
			m_animationStates.erase(it);
			OnAnimationChanged();
			return;
		}
	}
}

void AnimationNode::removeAnimationState(AnimationState* state) {
	for (auto it = m_animationStates.begin(); it != m_animationStates.end(); ++it) {
		if ((*it).get() == state) {
			m_animationStates.erase(it);
			OnAnimationChanged();
			return;
		}
	}
}

void AnimationNode::removeAnimationState(size_t index) {
	if (index < m_animationStates.size()) {
		m_animationStates.erase(m_animationStates.begin() + index);
		OnAnimationChanged();
	}
}

void AnimationNode::removeAllAnimationStates() {
	if (m_animationStates.size()) {
		m_animationStates.clear();
		OnAnimationChanged();
	}
}

void AnimationNode::OnAnimationOrderChanged() {
	m_animationOrderDirty = true;
	m_animationDirty = true;
	if (m_octree && m_octant && !m_reinsertQueued)
		m_octree->queueUpdate(this);
}

void AnimationNode::OnAnimationChanged(){
	m_animationDirty = true;
	if (m_octree && m_octant && !m_reinsertQueued)
		m_octree->queueUpdate(this);
}

unsigned short AnimationNode::getNumBones() {
	return m_numBones;
}

const Matrix4f* AnimationNode::getSkinMatrices() {
	return m_skinMatrices;
}

void AnimationNode::createBones() {
	m_numBones = static_cast<unsigned short>(meshBones.size());
	m_skinMatrices = new Matrix4f[m_numBones];
	m_bones = new BoneNode*[m_numBones];

	for (size_t i = 0; i < m_numBones; ++i) {
		const MeshBone& meshBone = meshBones[i];
		m_bones[i] = new BoneNode();
		m_bones[i]->setName(meshBone.name);
		m_bones[i]->setPosition(meshBone.initialPosition);
		m_bones[i]->setOrientation({ meshBone.initialRotation[0], meshBone.initialRotation[1], meshBone.initialRotation[2], meshBone.initialRotation[3] });
		m_bones[i]->setScale(meshBone.initialScale);
		m_bones[i]->m_offsetMatrix = meshBone.offsetMatrix;
	}

	for (size_t i = 0; i < m_numBones; ++i) {
		const MeshBone& meshBone = meshBones[i];

		if (meshBone.parentIndex == i) {
			m_bones[i]->setParent(this);
			m_rootBone = m_bones[i];
			m_bones[i]->setIsRootBone(true);
		}else {
			m_bones[i]->setParent(m_bones[meshBone.parentIndex]);
			//m_bones[i]->setOrigin(m_bones[i]->getPosition());
		}
	}

	for (size_t i = 0; i < m_numBones; ++i)
		m_bones[i]->countChildBones();
}

void AnimationNode::setUpdateSilent(bool updateSilent) {
	m_updateSilent = updateSilent;
}

BoneNode* AnimationNode::getRootBone() {
	return m_rootBone;
}