#include "Md2Node.h"
#include "../octree/Octree.h"

Md2Node::Md2Node(const Md2Model& md2Model) :
	OctreeNode(),
	md2Model(md2Model),
	m_animationDirty(true),
	m_disabled(false),
	m_materialIndex(-1),
	m_textureIndex(-1),
	m_activeFrameIdx(0),
	currentAnimation(&md2Model.getAnimations()[0]),
	m_speed(1.0f),
	m_activeFrame(0.0f),
	m_shader(nullptr),
	m_loopAnimation(true),
	OnAnimationEnd(nullptr),
	m_animationType(AnimationType::STAND),
	m_color(Vector4f::ONE){
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

	if (m_animationType == AnimationType::NONE)
		return;

	if (frameNumber && wasInView(*frameNumber)) {

		m_activeFrame += m_speed * dt * currentAnimation->fps;

		int len = static_cast<int>(currentAnimation->frames.size()) - 1;
		float lenf = static_cast<float>(len);
		if (m_activeFrame >= lenf) {
			m_activeFrame = m_loopAnimation ? std::modf(m_activeFrame, &m_activeFrame) + std::fmod(m_activeFrame, lenf + 1.0f) : lenf;
			if (OnAnimationEnd)
				OnAnimationEnd();
		}

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
	if (m_disabled)
		return;

	bool change = !(m_color == Vector4f::ONE);
	if (m_shader) {
		m_shader->use();
		m_shader->loadMatrix("u_model", getWorldTransformation());
		if (change)
			m_shader->loadVector("u_color", m_color);
	}

	md2Model.updateBuffer(m_interpolated);
	md2Model.draw(m_textureIndex, m_materialIndex);

	if (m_shader) {
		if (change)
			m_shader->loadVector("u_color", Vector4f::ONE);
		m_shader->unuse();
	}
}

void Md2Node::drawShadow() const {
	if (m_disabled)
		return;

	md2Model.updateBuffer(m_interpolated);
	md2Model.draw();
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

void Md2Node::setAnimationType(AnimationType animationType, AnimationType animationTypeN) {
	if (m_animationType == animationType || m_animationType == AnimationType::DEATH_BACK || m_animationType == AnimationType::DEATH_BACK_SLOW || m_animationType == AnimationType::DEATH_FORWARD)
		return;
	
	m_animationType = animationType;
	m_activeFrame = 0.0f;
	if (m_animationType == AnimationType::NONE) {	
		currentAnimation = &md2Model.getAnimations()[animationTypeN];
		const Utils::MD2IO::Frame& frame = currentAnimation->frames[0];
		std::copy(frame.vertices.begin(), frame.vertices.end(), m_interpolated.begin());		
		return;
	}
	currentAnimation = &md2Model.getAnimations()[m_animationType];
}

void Md2Node::setShader(Shader* shader) {
	m_shader = shader;
}

void Md2Node::setSpeed(float speed) {
	m_speed = speed;
}

void Md2Node::setLoopAnimation(bool loopAnimation) {
	m_loopAnimation = loopAnimation;
}

void Md2Node::setColor(const Vector4f& color) {
	m_color = color;
}

void Md2Node::setOnAnimationEnd(std::function<void()> fun) {
	OnAnimationEnd = fun;
}

void Md2Node::setDisabled(bool disabled) {
	m_disabled = disabled;
}