#include "Wall.h"

Wall::Wall(Vector2f position, Vector2f size) {
	InitCollider(position, size);
}

Wall::~Wall() {

}

Collision Wall::GetCollider() {
	return m_collider;
}

void Wall::InitCollider(const Vector2f& position, const Vector2f& size) {
	m_collider.size = size;
	m_collider.position = position;
}
