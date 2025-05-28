#include "Md2Node.h"
#include "../octree/Octree.h"

Md2Node::Md2Node(const Md2Model& md2Model) :
	OctreeNode(),
	md2Model(md2Model),
	m_animationDirty(true),
	m_materialIndex(-1),
	m_textureIndex(-1),
	m_activeFrameIdx(0),
	currentAnimation(&md2Model.getAnimations()[0]),
	m_speed(1.0f),
	m_activeFrame(0.0f),
	m_shader(nullptr),
	m_animationType(AnimationType::STAND) {
	m_interpolated.resize(md2Model.getNumVertices());
}

const Md2Model& Md2Node::getMd2Model() const {
	return md2Model;
}

void Md2Node::addChild(Md2Node* node, bool drawDebug) {
	OctreeNode::addChild(node);
	node->setDrawDebug(drawDebug);
}

void Md2Node::OnWorldBoundingBoxUpdate() const {
	m_worldBoundingBox = getLocalBoundingBox().transformed(getWorldTransformation());
}

void Md2Node::OnAnimationChanged() {
	m_animationDirty = true;
	if (m_octree && m_octant && !m_reinsertQueued)
		m_octree->queueUpdate(this);
}

void Md2Node::update(float dt) {
	if (frameNumber && wasInView(*frameNumber)) {

		// increment frame
		m_activeFrame += m_speed * dt * currentAnimation->fps;

		int len = static_cast<int>(currentAnimation->frames.size()) - 1;
		float lenf = static_cast<float>(len);

		// loop animation
		if (m_activeFrame >= lenf)
			m_activeFrame = std::modf(m_activeFrame, &m_activeFrame) + std::fmod(m_activeFrame, lenf + 1.0f);

		m_activeFrameIdx = static_cast<short>(m_activeFrame);
		short nextFrame = m_activeFrameIdx == len ? 0 : m_activeFrameIdx + 1;

		float lerp = m_activeFrame - floor(m_activeFrame);
		float oneMinusLerp = 1.0f - lerp;

		const Utils::MD2IO::Frame& frameA = currentAnimation->frames[m_activeFrameIdx];
		const Utils::MD2IO::Frame& frameB = currentAnimation->frames[nextFrame];

		std::transform(frameA.vertices.begin(), frameA.vertices.end(), frameB.vertices.begin(),
			m_interpolated.begin(), [lerp, oneMinusLerp](const Utils::MD2IO::Vertex& vert1, const Utils::MD2IO::Vertex& vert2) {
				return Utils::MD2IO::Vertex::Lerp(vert1, vert2, lerp, oneMinusLerp);
			}
		);

		OnAnimationChanged();
		if (m_animationDirty) {
			updateAnimation();
			OnBoundingBoxChanged();
			OnWorldBoundingBoxUpdate();
		}
	}
}

void Md2Node::updateAnimation() {	
	m_animationDirty = false;
}

void  Md2Node::drawRaw() const {
	if (m_shader) {
		m_shader->use();
		m_shader->loadMatrix("u_model", getWorldTransformation());
	}

	
	md2Model.updateBuffer(m_interpolated);
	md2Model.draw(m_textureIndex, m_materialIndex);

	if (m_shader)
		m_shader->unuse();
}

const BoundingBox& Md2Node::getLocalBoundingBox() const {
	return currentAnimation->frames[m_activeFrameIdx].boundingBox;
}

short Md2Node::getMaterialIndex() const {
	return m_materialIndex;
}

void Md2Node::setMaterialIndex(short index) {
	m_materialIndex = index;
}

short Md2Node::getTextureIndex() const {
	return m_textureIndex;
}

void Md2Node::setTextureIndex(short index) {
	m_textureIndex = index;
}

void Md2Node::setAnimationType(AnimationType animationType) {
	if (m_animationType == animationType)
		return;
	m_animationType = animationType;
	currentAnimation = &md2Model.getAnimations()[m_animationType];
	m_activeFrame = 0.0f;
}

void Md2Node::setShader(Shader* shader) {
	m_shader = shader;
}