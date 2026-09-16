#include "Enemy.h"

Enemy::Enemy(btCollisionObject* collisionObject) : CollisionNode(collisionObject){
    m_collisionObject->setUserPointer(this);
}

Enemy::~Enemy() {

}

void Enemy::update(const float dt) {

}

void Enemy::fixedUpdate(float fdt) {
    if (!m_isActive) return;

    const Vector3f& pos = getPosition();
    const Quaternion& rot = getOrientation();
    m_collisionObject->setWorldTransform(Physics::BtTransform(pos, rot));
}