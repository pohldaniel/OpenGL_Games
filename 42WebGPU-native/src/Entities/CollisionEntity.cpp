#include <iostream>
#include "CollisionEntity.h"

CollisionEntity::CollisionEntity(btCollisionObject* collisionObject) : CollisionNode(collisionObject){

}

CollisionEntity::~CollisionEntity() {

}

void CollisionEntity::update(const float dt) {

}

void CollisionEntity::setPosition(const float x, const float y, const float z) const {
    btTransform transform = m_collisionObject->getWorldTransform();
    transform.setOrigin(btVector3(x, y, z));
    
    m_collisionObject->setWorldTransform(transform);
}

void CollisionEntity::setOrientation(const float x, const float y, const float z, const float w) const {
    btTransform transform = m_collisionObject->getWorldTransform();
    transform.setRotation(btQuaternion(x, y, z, w));

    m_collisionObject->setWorldTransform(transform);
}

void CollisionEntity::fixedUpdate(float fdt) {
    if (!m_isActive) return;

    const Vector3f& pos = getPosition();
    const Quaternion& rot = getOrientation();
    m_collisionObject->setWorldTransform(Physics::BtTransform(pos, rot));
}