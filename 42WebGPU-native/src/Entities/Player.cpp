#include <iostream>
#include "Player.h"

Player::Player(btCollisionObject* collisionObject, AnimatedModel& model) : CollisionNode(collisionObject), model(model){
    m_collisionObject->setUserPointer(this);
}

Player::~Player() {

}

void Player::update(float dt) {
   
}

void Player::fixedUpdate(float fdt) {
    if (!m_isActive) return;

    const Vector3f& pos = getPosition();   
    m_collisionObject->setWorldTransform(Physics::BtTransform(pos + Vector3f(0.0f, 0.4f, 0.0f)));
}

void Player::translate(float dx, float dy, float dz) {
    model.translate(dx, dy, dz);
}

const Vector3f& Player::getPosition() const {
    return static_cast<const AnimatedMesh*>(model.getMesh())->getRootBone()->getPosition();
}