#include "WidgetMH.h"

NodeUI::NodeUI() : m_parent(nullptr), m_index(-1) {

}

NodeUI::NodeUI(const NodeUI& rhs) {
	m_children = rhs.m_children;
	for (auto& children : m_children) {
		children->m_parent = this;
	}
	m_parent = rhs.m_parent;
	m_nameHash = rhs.m_nameHash;
	m_index = rhs.m_index;
}

NodeUI::NodeUI(NodeUI&& rhs) {
	m_children = rhs.m_children;
	for (auto& children : m_children) {
		children->m_parent = this;
	}
	m_parent = rhs.m_parent;
	m_nameHash = rhs.m_nameHash;
	m_index = rhs.m_index;
}

NodeUI::~NodeUI() {
	eraseAllChildren();
}

std::list<std::shared_ptr<NodeUI>>& NodeUI::getChildren() const {
	return m_children;
}

void NodeUI::eraseChild(NodeUI* child) {
	if (!child || child->m_parent != this)
		return;

	child->m_parent = nullptr;
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child](const std::shared_ptr<NodeUI>& node) { return node.get() == child; }), m_children.end());
}

void NodeUI::eraseChild(const int index) {
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [index](const std::shared_ptr<NodeUI>& node) { return node->getIndex() == index; }), m_children.end());
}

NodeUI* NodeUI::addChild(NodeUI* node) {
	m_children.emplace_back(std::shared_ptr<NodeUI>(node));
	m_children.back()->m_parent = this;
	return m_children.back().get();
}

NodeUI* NodeUI::getParent() const {
	return m_parent;
}

const int NodeUI::getIndex() const {
	return m_index;
}

void NodeUI::setParent(NodeUI* node) {

	if (node && m_parent) {
		std::list<std::shared_ptr<NodeUI>>::iterator it = std::find_if(m_parent->getChildren().begin(), m_parent->getChildren().end(), [node](std::shared_ptr<NodeUI>& _node) { return _node.get() == node; });

		if (it != m_parent->getChildren().end()) {
			node->getChildren().splice(m_parent->getChildren().end(), m_parent->getChildren(), it);
		}
	}else if (node) {
		node->addChild(this);
	}else if (m_parent) {
		m_parent->eraseChild(this);
	}
}

void NodeUI::setName(const std::string& name) {
	m_nameHash = StringHash(name);
}

void NodeUI::setIndex(const int index) {
	m_index = index;
}

void NodeUI::eraseAllChildren(size_t offset) {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {

		if (!(*it)->getChildren().empty()) 
			(*it)->eraseAllChildren();
		(*it)->m_parent = nullptr;	
	}
	//m_children.clear();
	m_children.erase(std::next(m_children.begin(), offset), m_children.end());
}

size_t NodeUI::countNodes() {
	size_t num = m_children.size();
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		num += (*it)->countNodes();
	}

	return num;
}

/////////////////////////////////////////////////////////////////////////////////////////////////
Vector2f WidgetMH::WorldPosition;
Vector2f WidgetMH::WorldScale;
float WidgetMH::WorldOrientation;

WidgetMH::WidgetMH() : NodeUI(), Sprite(), m_isDirty(true) {

}

WidgetMH::WidgetMH(const WidgetMH& rhs) : NodeUI(rhs), Sprite(rhs) {
	m_isDirty = rhs.m_isDirty;
}

WidgetMH::WidgetMH(WidgetMH&& rhs) : NodeUI(rhs), Sprite(rhs) {
	m_isDirty = rhs.m_isDirty;
}

WidgetMH::~WidgetMH() {

}

void WidgetMH::draw() {
	
}

void WidgetMH::OnTransformChanged() {

	if (m_isDirty) {
		return;
	}

	for (auto& child : m_children) {
		std::static_pointer_cast<WidgetMH>(child)->OnTransformChanged();
	}

	m_isDirty = true;
}

const Matrix4f& WidgetMH::getWorldTransformation() const {
	if (m_isDirty) {
		if (m_parent) {
			m_modelMatrix = static_cast<WidgetMH*>(m_parent)->getWorldTransformation() * getTransformationSOP();
		}else {
			m_modelMatrix = getTransformationSOP();
		}
		m_isDirty = false;
	}

	return m_modelMatrix;
}

const Matrix4f WidgetMH::getWorldTransformationWithTranslation(const Vector2f& trans) const {
	Matrix4f mtx = getWorldTransformation();
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		float dx = trans[0] * invScale[0];
		float dy = trans[1] * invScale[1];
		mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
		mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];
	}else {
		mtx[3][0] = mtx[3][0] + trans[0] * mtx[0][0] + trans[1] * mtx[1][0];
		mtx[3][1] = mtx[3][1] + trans[0] * mtx[0][1] + trans[1] * mtx[1][1];
	}
	return mtx;	
}

const Matrix4f WidgetMH::getWorldTransformationWithTranslation(float dx, float dy) const {
	Matrix4f mtx = getWorldTransformation();
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());	
		dx = dx * invScale[0];
		dy = dy * invScale[1];
		mtx[3][0] = mtx[3][0] + dx* mtx[0][0] + dy* mtx[1][0];
		mtx[3][1] = mtx[3][1] + dx* mtx[0][1] + dy* mtx[1][1];		
	}else {
		mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
		mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];
	}
	return mtx;
}

const Matrix4f WidgetMH::getWorldTransformationWithScale(float sx, float sy, bool relative) const {
	Matrix4f mtx = getWorldTransformation();
	if (m_parent && relative) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		sx = sx * invScale[0];
		sy = sy * invScale[1];		
		mtx[0][0] = mtx[0][0] * sx;  mtx[1][0] = mtx[1][0] * sy;
		mtx[0][1] = mtx[0][1] * sx;  mtx[1][1] = mtx[1][1] * sy;
	}else {
		mtx[0][0] = mtx[0][0] * sx;  mtx[1][0] = mtx[1][0] * sy;
		mtx[0][1] = mtx[0][1] * sx;  mtx[1][1] = mtx[1][1] * sy;
	}
	return mtx;
}

const Matrix4f WidgetMH::getWorldTransformationWithScale(const Vector2f& scale, bool relative) const {
	return getWorldTransformationWithScale(scale[0], scale[1], relative);
}

const Matrix4f WidgetMH::getWorldTransformationWithScale(const float s, bool relative) const {
	return getWorldTransformationWithScale(s, s, relative);
}

const Matrix4f WidgetMH::getWorldTransformationWithScaleAndTranslation(float sx, float sy, float dx, float dy, bool relative) const {
	Matrix4f mtx = getWorldTransformation();
	if (m_parent && relative) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		
		dx = dx * invScale[0];
		dy = dy * invScale[1];

		mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
		mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];

		sx = sx * invScale[0];
		sy = sy * invScale[1];

		mtx[0][0] = mtx[0][0] * sx;  mtx[1][0] = mtx[1][0] * sy;
		mtx[0][1] = mtx[0][1] * sx;  mtx[1][1] = mtx[1][1] * sy;

	}else {
		mtx[3][0] = mtx[3][0] + dx * mtx[0][0] + dy * mtx[1][0];
		mtx[3][1] = mtx[3][1] + dx * mtx[0][1] + dy * mtx[1][1];

		mtx[0][0] = mtx[0][0] * sx;  mtx[1][0] = mtx[1][0] * sy;
		mtx[0][1] = mtx[0][1] * sx;  mtx[1][1] = mtx[1][1] * sy;	
	}
	return mtx;
}

const Matrix4f WidgetMH::getWorldTransformationWithScaleAndTranslation(const Vector2f& scale, const Vector2f& trans, bool relative) const {
	return getWorldTransformationWithScaleAndTranslation(scale[0], scale[1], trans[0], trans[1], relative);
}

void WidgetMH::updateWorldTransformation() const {
	if (m_isDirty) {
		if (m_parent) {
			m_modelMatrix = static_cast<WidgetMH*>(m_parent)->getWorldTransformation() * getTransformationSOP();
		}else {
			m_modelMatrix = getTransformationSOP();
		}
		m_isDirty = false;
	}
}

const Vector2f& WidgetMH::getWorldPosition(bool update) const {
	if (update)
		WorldPosition = getWorldTransformation().getTranslation2D();
	return WorldPosition;
}

const Vector2f& WidgetMH::getWorldScale(bool update) const {
	if (update)
		WorldScale = getWorldTransformation().getScale2D();
	return WorldScale;
}

const float WidgetMH::getWorldOrientation(bool update) const {
	if (update)
		WorldOrientation = getWorldTransformation().getRotation2D().getRoll2D();
	return WorldOrientation;
}

void WidgetMH::setScale(const float sx, const float sy) {
	Sprite::setScale(sx, sy);
	OnTransformChanged();
}

void WidgetMH::setScale(const Vector2f& scale) {
	Sprite::setScale(scale);
	OnTransformChanged();
}

void WidgetMH::setScale(const float s) {
	Sprite::setScale(s);
	OnTransformChanged();
}

void WidgetMH::setScaleAbsolute(const float sx, const float sy) {
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::setScale(sx * invScale[0], sy * invScale[1]);
	}else {
		Sprite::setScale(sx, sy);
	}
	OnTransformChanged();
}

void WidgetMH::setScaleAbsolute(const Vector2f& scale) {
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::setScale(scale * invScale);
	}else {
		Sprite::setScale(scale);
	}
	OnTransformChanged();
}

void WidgetMH::setScaleAbsolute(const float s) {
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::setScale(s * invScale[0], s * invScale[1]);
	}else {
		Sprite::setScale(s);
	}
	OnTransformChanged();
}

void WidgetMH::setPosition(const float x, const float y) {
	Sprite::setPosition(x, y);
	OnTransformChanged();
}

void WidgetMH::setPosition(const Vector2f& position) {
	Sprite::setPosition(position);
	OnTransformChanged();
}

void WidgetMH::setOrigin(const float x, const float y) {
	Sprite::setOrigin(x, y);
	OnTransformChanged();
}

void WidgetMH::setOrigin(const Vector2f& origin) {
	Sprite::setOrigin(origin);
	OnTransformChanged();
}

void WidgetMH::setOrientation(float degrees) {
	Sprite::setOrientation(degrees);
	OnTransformChanged();
}

void WidgetMH::translate(const Vector2f& trans) {
	Sprite::translate(trans);
	OnTransformChanged();
}

void WidgetMH::translate(const float dx, const float dy) {
	Sprite::translate(dx, dy);
	OnTransformChanged();
}

void WidgetMH::translateRelative(const Vector2f& trans) {
	if (m_parent) {
		Sprite::translateRelative(trans * Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale()));
	}else {
		Sprite::translateRelative(trans);
	}
	OnTransformChanged();
}

void WidgetMH::translateRelative(const float dx, const float dy) {
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::translateRelative(dx * invScale[0], dy * invScale[1]);
	}else {
		Sprite::translateRelative(dx, dy);
	}
	OnTransformChanged();
}

void WidgetMH::scale(const Vector2f& scale) {
	Sprite::scale(scale);
	OnTransformChanged();
}

void WidgetMH::scale(const float sx, const float sy) {
	Sprite::scale(sx, sy);
	OnTransformChanged();
}

void WidgetMH::scale(const float s) {
	Sprite::scale(s);
	OnTransformChanged();
}

void WidgetMH::scaleAbsolute(const Vector2f& scale) {
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::scale(scale * invScale);
	}else {
		Sprite::scale(scale);
	}
	OnTransformChanged();
}

void WidgetMH::scaleAbsolute(const float sx, const float sy) {
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::scale(sx * invScale[0], sy * invScale[1]);
	}else {
		Sprite::scale(sx, sy);
	}
	OnTransformChanged();
}

void WidgetMH::scaleAbsolute(const float s) {
	if (m_parent) {
		const Vector2f& invScale = Vector2f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::scale(s * invScale[0], s * invScale[1]);
	}else {
		Sprite::scale(s);
	}
	OnTransformChanged();
}

void WidgetMH::rotate(const float degrees) {
	Sprite::rotate(degrees);
	OnTransformChanged();
}
