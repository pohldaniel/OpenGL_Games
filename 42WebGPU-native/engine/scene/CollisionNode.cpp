#include "CollisionNode.h"

CollisionNode::CollisionNode(btCollisionObject* collisionObject) : SceneNode(), m_collisionObject(collisionObject), m_isActive(true) {
	
}

CollisionNode::~CollisionNode() {
    Physics::DeleteCollisionObject(m_collisionObject);
}

void CollisionNode::addChild(CollisionNode* node) {
	SceneNode::addChild(node);
}

btCollisionObject* CollisionNode::getCollisionObject() const {
	return m_collisionObject;
}

const Matrix4f& CollisionNode::getWorldTransformation() const {

    const btRigidBody* body = btRigidBody::upcast(m_collisionObject);

    btTransform transform;
    if (body && body->getMotionState()) {
        body->getMotionState()->getWorldTransform(transform);
    }else {
        transform = m_collisionObject->getWorldTransform();
    }

    btVector3 pos = transform.getOrigin();
    setPosition(pos.x(), pos.y(), pos.z());

    btQuaternion quat = transform.getRotation();
    setOrientation(quat.x(), quat.y(), quat.z(), quat.w());

    return SceneNode::getWorldTransformation();
}

void CollisionNode::setActive(bool active) {
    m_isActive = active;
    if (m_isActive) {
        m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() & ~btCollisionObject::CF_NO_CONTACT_RESPONSE);
        m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() & ~btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        m_collisionObject->activate(true);
    }else {
        m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
        m_collisionObject->setCollisionFlags(m_collisionObject->getCollisionFlags() | btCollisionObject::CF_DISABLE_VISUALIZE_OBJECT);
        m_collisionObject->forceActivationState(ISLAND_SLEEPING); 
    }
}

bool CollisionNode::isActive() {
    return m_isActive;
}