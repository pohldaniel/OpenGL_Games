#include "Wall.h"

Wall::Wall(Vector2f position, Vector2f size) {
	initCollider(position, size);
}

Wall::~Wall() {

}

Collision Wall::getCollider() {
	return m_collider;
}

void Wall::initCollider(const Vector2f& position, const Vector2f& size) {
	m_collider.size = size;
	m_collider.position = position;
}
