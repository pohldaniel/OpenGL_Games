#pragma once

#include <engine/interfaces/ICounter.h>

#include "BaseNode.h"

class SceneNode : public BaseNode {

public:

	SceneNode();


	const Vector3f& getScalePosition() const override;
	const Vector3f& getWorldPosition() const override;
	const Vector3f& getWorldScale() const override;
	const Quaternion& getWorldOrientation() const override;

	const Matrix4f& getTransformation() const override;

private:

	void updateSOP() const;

	mutable Vector3f m_worldPosition;
	mutable Vector3f m_worldScale;
	mutable Quaternion m_worldOrientation;
	mutable Vector3f m_scalePosition;
	mutable Matrix4f m_modelMatrix;
};