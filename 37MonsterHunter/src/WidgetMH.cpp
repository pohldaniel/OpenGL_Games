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

WidgetMH::WidgetMH() : ObjectMH(), m_isDirty(true) {

}

WidgetMH::WidgetMH(const WidgetMH& rhs) : NodeMH(rhs), ObjectMH(rhs) {
	m_isDirty = rhs.m_isDirty;
}

WidgetMH& WidgetMH::operator=(const WidgetMH& rhs) {
	NodeMH::operator=(rhs);
	ObjectMH::operator=(rhs);
	m_isDirty = rhs.m_isDirty;
	return *this;
}

WidgetMH::WidgetMH(WidgetMH&& rhs) : NodeMH(rhs), ObjectMH(rhs) {
	m_isDirty = rhs.m_isDirty;
}

WidgetMH& WidgetMH::operator=(WidgetMH&& rhs) {
	NodeMH::operator=(rhs);
	ObjectMH::operator=(rhs);
	m_isDirty = rhs.m_isDirty;
	return *this;
}

WidgetMH::~WidgetMH() {

}

void WidgetMH::draw() {
	if (m_children.size() > 0) {
		for (auto&& child : m_children) {
			static_cast<WidgetMH*>(child.get())->draw();
		}
	}
}

void WidgetMH::processInput() {
	if (m_children.size() > 0) {
		for (auto&& child : m_children) {
			static_cast<WidgetMH*>(child.get())->processInput();
		}
	}
}

const Vector2f& WidgetMH::getWorldPosition(bool update) const {
	if (update)
		updateSOP();
	return m_worldPosition;
}

void WidgetMH::OnTransformChanged() {

	if (m_isDirty) {
		return;
	}

	for (auto&& child : m_children) {
		child->OnTransformChanged();
	}

	m_isDirty = true;
}

const Vector2f& WidgetMH::getWorldScale(bool update) const {
	if (update)
		updateSOP();
	return m_worldScale;
}

const float WidgetMH::getWorldOrientation(bool update) const {
	if (update)
		updateSOP();
	return m_worldOrientation;
}

void WidgetMH::updateSOP() const {
	if (m_isDirty) {
		if (m_parent) {
			m_worldPosition = m_parent->getWorldPosition() + m_position * m_parent->getWorldScale();		
			m_worldScale = m_parent->getWorldScale() * m_scale;
			m_worldOrientation = m_parent->getWorldOrientation() + m_orientation;
		}else {
			m_worldPosition = m_position;
			m_worldScale = m_scale;
			m_worldOrientation = m_orientation;
		}
		m_isDirty = false;
	}
}