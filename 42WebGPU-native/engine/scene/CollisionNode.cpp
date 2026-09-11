#include "CollisionNode.h"
#include <iostream>
CollisionNode::CollisionNode(btCollisionObject* collisionObject) : SceneNode(), m_collisionObject(collisionObject) {
	
}

CollisionNode::~CollisionNode() {

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