#include "GameObject.h"

const Vector2f &GameObject::getPosition() {
	return m_position;
}

const Matrix4f &GameObject::getTransform() {
	return m_transform;
}