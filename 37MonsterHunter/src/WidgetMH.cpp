#include "WidgetMH.h"

ObjectMH::ObjectMH() {
	m_scale.set(1.0f, 1.0f);
	m_position.set(0.0f, 0.0f);
}

ObjectMH::ObjectMH(ObjectMH const& rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
}

ObjectMH& ObjectMH::operator=(const ObjectMH& rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
	return *this;
}

ObjectMH::ObjectMH(ObjectMH&& rhs) : ObjectMH(rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
}

ObjectMH& ObjectMH::operator=(ObjectMH&& rhs) {
	m_position = rhs.m_position;
	m_scale = rhs.m_scale;
	return *this;
}

ObjectMH::~ObjectMH() {

}

void ObjectMH::setScale(const float sx, const float sy) {
	m_scale.set(sx, sy);
}

void ObjectMH::setScale(const Vector2f &scale) {
	m_scale = scale;
}

void ObjectMH::setScale(const float s) {
	setScale(s, s);
}

void ObjectMH::setPosition(const float x, const float y) {
	m_position.set(x, y);
}

void ObjectMH::setPosition(const Vector2f &position) {
	m_position = position;
}

void ObjectMH::setOrientation(const float degrees) {
	m_orientation = degrees;
}

void ObjectMH::translate(const Vector2f &trans) {
	m_position.translate(trans);
}

void ObjectMH::translate(const float dx, const float dy) {
	m_position.translate(dx, dy);
}

void ObjectMH::rotate(const float degrees) {
	m_orientation += degrees;
}

void ObjectMH::translateRelative(const Vector2f& trans) {

	float angle = m_orientation * HALF_PI_ON_180;
	float cos = cosf(angle);
	float sin = sinf(angle);

	float dx = trans[0] * cos - trans[1] * sin;
	float dy = trans[0] * sin + trans[1] * cos;

	m_position += Vector2f(dx, dy);
}

void ObjectMH::translateRelative(const float _dx, const float _dy) {

	float angle = m_orientation * HALF_PI_ON_180;
	float cos = cosf(angle);
	float sin = sinf(angle);

	float dx = _dx * cos - _dy * sin;
	float dy = _dx * sin + _dy * cos;

	m_position[0] += dx;
	m_position[1] += dy;
}

void ObjectMH::scale(const Vector2f &scale) {
	m_scale.scale(scale);
}

void ObjectMH::scale(const float sx, const float sy) {
	m_scale.scale(sx, sy);
}

void ObjectMH::scale(const float s) {
	m_scale.scale(s, s);
}

const Vector2f& ObjectMH::getPosition() const {
	return m_position;
}

const Vector2f& ObjectMH::getScale() const {
	return m_scale;
}

const float ObjectMH::getOrientation() const {
	return m_orientation;
}

NodeMH::NodeMH() : m_parent(nullptr) {

}

NodeMH::NodeMH(const NodeMH& rhs) {
	m_children = rhs.m_children;
	for (auto& children : m_children) {
		children->m_parent = this;
	}
	m_parent = rhs.m_parent;
}

NodeMH::NodeMH(NodeMH&& rhs) {
	m_children = rhs.m_children;
	for (auto& children : m_children) {
		children->m_parent = this;
	}
	m_parent = rhs.m_parent;
}

NodeMH& NodeMH::operator=(const NodeMH& rhs) {
	m_children = rhs.m_children;
	for (auto& children : m_children) {
		children->m_parent = this;
	}
	m_parent = rhs.m_parent;
	return *this;
}

NodeMH& NodeMH::operator=(NodeMH&& rhs) {
	m_children = rhs.m_children;
	for (auto& children : m_children) {
		children->m_parent = this;
	}
	m_parent = rhs.m_parent;
	return *this;
}

NodeMH::~NodeMH() {

}

std::list<std::shared_ptr<NodeMH>>& NodeMH::getChildren() const {
	return m_children;
}

void NodeMH::eraseChild(NodeMH* child) {
	if (!child || child->m_parent != this)
		return;

	child->m_parent = nullptr;
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child](const std::shared_ptr<NodeMH>& node) { return node.get() == child; }), m_children.end());
}

NodeMH* NodeMH::addChild(NodeMH* node) {
	m_children.emplace_back(std::shared_ptr<NodeMH>(node));
	m_children.back()->m_parent = this;
	return m_children.back().get();
}

NodeMH* NodeMH::getParent() const {
	return m_parent;
}

void NodeMH::setParent(NodeMH* node) {

	if (node && m_parent) {
		std::list<std::shared_ptr<NodeMH>>::iterator it = std::find_if(m_parent->getChildren().begin(), m_parent->getChildren().end(), [node](std::shared_ptr<NodeMH>& _node) { return _node.get() == node; });

		if (it != m_parent->getChildren().end()) {
			node->getChildren().splice(m_parent->getChildren().end(), m_parent->getChildren(), it);
		}
	}else if (node) {
		node->addChild(this);
	}else if (m_parent) {
		m_parent->eraseChild(this);
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////
Vector3f WidgetMH::WorldPosition;
Vector3f WidgetMH::WorldScale;
Quaternion WidgetMH::WorldOrientation;

WidgetMH::WidgetMH() : NodeMH(), Sprite(), m_isDirty(true) {

}

WidgetMH::WidgetMH(const WidgetMH& rhs) : NodeMH(rhs), Sprite(rhs) {
	m_isDirty = rhs.m_isDirty;
}

WidgetMH& WidgetMH::operator=(const WidgetMH& rhs) {
	NodeMH::operator=(rhs);
	Sprite::operator=(rhs);
	m_isDirty = rhs.m_isDirty;
	return *this;
}

WidgetMH::WidgetMH(WidgetMH&& rhs) : NodeMH(rhs), Sprite(rhs) {
	m_isDirty = rhs.m_isDirty;
}

WidgetMH& WidgetMH::operator=(WidgetMH&& rhs) {
	NodeMH::operator=(rhs);
	Sprite::operator=(rhs);
	m_isDirty = rhs.m_isDirty;
	return *this;
}

WidgetMH::~WidgetMH() {

}

/*void WidgetMH::draw() {
	if (m_children.size() > 0) {
		for (auto& child : m_children) {
			std::static_pointer_cast<WidgetMH>(child)->draw();
		}
	}
}

void WidgetMH::processInput() {
	if (m_children.size() > 0) {
		for (auto& child : m_children) {
			std::static_pointer_cast<WidgetMH>(child)->processInput();
		}
	}
}*/

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

const Vector3f& WidgetMH::getWorldPosition(bool update) const {
	if (update)
		WorldPosition = getWorldTransformation().getTranslation();
	return WorldPosition;
}

const Vector3f& WidgetMH::getWorldScale(bool update) const {
	if (update)
		WorldScale = getWorldTransformation().getScale();
	return WorldScale;
}

const Quaternion& WidgetMH::getWorldOrientation(bool update) const {
	if (update)
		WorldOrientation = Quaternion(getWorldTransformation().getRotation());
	return WorldOrientation;
}

void WidgetMH::setScale(const float sx, const float sy, const float sz) {
	Sprite::setScale(sx, sy, sz);
	OnTransformChanged();
}

void WidgetMH::setScale(const Vector3f& scale) {
	Sprite::setScale(scale);
	OnTransformChanged();
}

void WidgetMH::setScale(const float s) {
	Sprite::setScale(s);
	OnTransformChanged();
}

void WidgetMH::setPosition(const float x, const float y, const float z) {
	Sprite::setPosition(x, y, z);
	OnTransformChanged();
}

void WidgetMH::setPosition(const Vector3f& position) {
	Sprite::setPosition(position);
	OnTransformChanged();
}

void WidgetMH::setOrigin(const float x, const float y, const float z) {
	Sprite::setOrigin(x, y, z);
	OnTransformChanged();
}

void WidgetMH::setOrigin(const Vector3f& origin) {
	Sprite::setOrigin(origin);
	OnTransformChanged();
}

void WidgetMH::setOrientation(const Vector3f& axis, float degrees) {
	Sprite::setOrientation(axis, degrees);
	OnTransformChanged();
}

void WidgetMH::setOrientation(const float degreesX, const float degreesY, const float degreesZ) {
	Sprite::setOrientation(degreesX, degreesY, degreesZ);
	OnTransformChanged();
}

void WidgetMH::setOrientation(const Vector3f& euler) {
	Sprite::setOrientation(euler);
	OnTransformChanged();
}

void WidgetMH::setOrientation(const Quaternion& orientation) {
	Sprite::setOrientation(orientation);
	OnTransformChanged();
}

void WidgetMH::setOrientation(const float x, const float y, const float z, const float w) {
	Sprite::setOrientation(x, y, z, w);
	OnTransformChanged();
}

void WidgetMH::translate(const Vector3f& trans) {
	Sprite::translate(trans);
	OnTransformChanged();
}

void WidgetMH::translate(const float dx, const float dy, const float dz) {
	Sprite::translate(dx, dy, dz);
	OnTransformChanged();
}

void WidgetMH::translateRelative(const Vector3f& trans) {
	if (m_parent) {
		Sprite::translateRelative(trans * Vector3f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale()));
	}else {
		Sprite::translateRelative(trans);
	}
	OnTransformChanged();
}

void WidgetMH::translateRelative(const float dx, const float dy, const float dz) {
	if (m_parent) {
		const Vector3f& invScale = Vector3f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::translateRelative(dx * invScale[0], dy * invScale[1], dz * invScale[2]);
	}else {
		Sprite::translateRelative(dx, dy, dz);
	}
	OnTransformChanged();
}

void WidgetMH::scale(const Vector3f& scale) {
	Sprite::scale(scale);
	OnTransformChanged();
}

void WidgetMH::scale(const float sx, const float sy, const float sz) {
	Sprite::scale(sx, sy, sz);
	OnTransformChanged();
}

void WidgetMH::scale(const float s) {
	Sprite::scale(s);
	OnTransformChanged();
}

void WidgetMH::scaleAbsolute(const Vector3f& scale) {
	if (m_parent) {
		const Vector3f& invScale = Vector3f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::scale(scale * invScale);
	}else {
		Sprite::scale(scale);
	}
	OnTransformChanged();
}

void WidgetMH::scaleAbsolute(const float sx, const float sy, const float sz) {
	if (m_parent) {
		const Vector3f& invScale = Vector3f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::scale(sx * invScale[0], sy * invScale[1], sz * invScale[2]);
	}else {
		Sprite::scale(sx, sy, sz);
	}
	OnTransformChanged();
}

void WidgetMH::scaleAbsolute(const float s) {
	if (m_parent) {
		const Vector3f& invScale = Vector3f::Inverse(static_cast<WidgetMH*>(m_parent)->getWorldScale());
		Sprite::scale(s * invScale[0], s * invScale[1], s * invScale[2]);
	}else {
		Sprite::scale(s);
	}
	OnTransformChanged();
}

void WidgetMH::rotate(const float pitch, const float yaw, const float roll) {
	Sprite::rotate(pitch, yaw, roll);
	OnTransformChanged();
}

void WidgetMH::rotate(const Vector3f& eulerAngle) {
	Sprite::rotate(eulerAngle);
	OnTransformChanged();
}

void WidgetMH::rotate(const Vector3f& axis, float degrees) {
	Sprite::rotate(axis, degrees);
	OnTransformChanged();
}

void WidgetMH::rotate(const Quaternion& orientation) {
	Sprite::rotate(orientation);
	OnTransformChanged();
}

void WidgetMH::rotate(const float x, const float y, const float z, const float w) {
	Sprite::rotate(x, y, z, w);
	OnTransformChanged();
}