#pragma once

#include <engine/interfaces/ICounter.h>

#include "IObjectNode.h"

class ObjectNode : public IObjectNode<ObjectNode> {

public:

	ObjectNode();

	virtual const Vector3f& getPosition() const override;
	virtual const Vector3f& getScale() const override;
	virtual const Quaternion& getOrientation() const override;

private:

	void updateSOP() const;

	mutable Vector3f worldPosition;
	mutable Vector3f worldScale;
	mutable Quaternion worldOrientation;
};

class ObjectNode2 : public ObjectNode {

public:

	ObjectNode2();
};