#include "Enemy.h"

Enemy::Enemy(btCollisionObject* collisionObject, const Vector3f& target) : CollisionNode(collisionObject), target(target), m_isDeath(false){
    m_collisionObject->setUserPointer(this);
}

Enemy::~Enemy() {

}

void Enemy::update(float dt) {
    const float monsterSpeed = 0.6f;
    float distanceSq = (target - getPosition()).lengthSq();

    if (distanceSq < 0.35f)
        return;

    Quaternion rot = Quaternion::Rotate(0.0f, getLookAtYRotation(getPosition(), target), 0.0f);
    setOrientation(rot);
    translateRelative(Vector3f::FORWARD * dt * monsterSpeed);
}

void Enemy::fixedUpdate(float fdt) {
    if (!m_isActive) return;

    const Vector3f& pos = getPosition();
    const Quaternion& rot = getOrientation();

    m_collisionObject->setWorldTransform(Physics::BtTransform(pos, rot));
}

float Enemy::getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos) {
    float dx = targetPos[0] - objectPos[0];
    float dz = targetPos[2] - objectPos[2];

    if (abs(dx) < 0.01f && abs(dz) < 0.01f)
        return 0.0f;

    return std::atan2(dx, dz) * _180_ON_PI;
}

const Vector3f Enemy::getDirection() const {
    return Quaternion::Rotate(getOrientation(), Vector3f::FORWARD);
}

void Enemy::setIsDeath(bool isDeath) {
    m_isDeath = true;
}

bool Enemy::isDeath() {
    return m_isDeath;
}