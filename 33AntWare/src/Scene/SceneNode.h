#pragma once

#include <engine/interfaces/ICounter.h>

#include "BaseNode.h"

class SceneNode : public BaseNode {

public:

	SceneNode();

	const Vector3f& getPosition() const override;
	const Vector3f& getScale() const override;
	const Vector3f& getScalePosition() const override;
	const Quaternion& getOrientation() const override;

	const Vector3f& getLocalPosition() const;
	const Vector3f& getLocalScale() const;
	const Quaternion& getLocalOrientation() const;

	const Matrix4f& getTransformation() const override;
	//const Matrix4f& getTransformationOld() const;

private:

	void updateSOP() const;

	mutable Vector3f m_worldPosition;
	mutable Vector3f m_worldScale;
	mutable Quaternion m_worldOrientation;
	mutable Vector3f m_scalePosition;
	//mutable Matrix4f m_modelMatrix;
};