#include <iostream>
#include "CollisionEntity.h"

CollisionEntity::CollisionEntity(btCollisionObject* collisionObject) : CollisionNode(collisionObject){

}

CollisionEntity::~CollisionEntity() {

}

void CollisionEntity::update(const float dt) {

}

void CollisionEntity::fixedUpdate(float fdt) {
    if (!m_isActive) return;

    const Vector3f& pos = getPosition();
    const Quaternion& rot = getOrientation();
    m_collisionObject->setWorldTransform(Physics::BtTransform(pos, rot));
}