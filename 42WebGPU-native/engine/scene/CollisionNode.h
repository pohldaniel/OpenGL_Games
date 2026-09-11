#pragma once

#include <Physics/Physics.h>
#pragma once

#include "SceneNode.h"

class CollisionNode : public SceneNode {

public:

	CollisionNode(btCollisionObject* collisionObject);
	~CollisionNode();

	const Matrix4f& getWorldTransformation() const override;

	using SceneNode::addChild;
	void addChild(CollisionNode* node);
	btCollisionObject* getCollisionObject() const;

protected:

	btCollisionObject* m_collisionObject;
};