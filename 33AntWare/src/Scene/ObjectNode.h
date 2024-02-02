#pragma once

#include <engine/interfaces/ICounter.h>

#include "BaseNode.h"

class ObjectNode : public BaseNode {

public:

	ObjectNode();

	const Vector3f& getPosition() const override;
	const Vector3f& getScale() const override;
	const Quaternion& getOrientation() const override;

private:

	void updateSOP() const;

	mutable Vector3f worldPosition;
	mutable Vector3f worldScale;
	mutable Quaternion worldOrientation;
};