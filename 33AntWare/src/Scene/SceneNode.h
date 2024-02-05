#pragma once

#include <engine/interfaces/ICounter.h>

#include "BaseNode.h"

class SceneNode : public BaseNode {

public:

	SceneNode();
	SceneNode(const SceneNode& rhs);
	SceneNode& operator=(const SceneNode& rhs);
	SceneNode(SceneNode&& rhs);
	SceneNode& operator=(SceneNode&& rhs);

	const Matrix4f& getWorldTransformation() const override;
	const Vector3f& getWorldPosition() const override;
	const Vector3f& getWorldScale() const override;
	const Vector3f& getWorldOrigin() const override;
	const Quaternion& getWorldOrientation() const override;

protected:

	bool m_isSubroot;

private:

	void updateSOP() const;
	mutable Vector3f m_worldPosition;
	mutable Vector3f m_worldScale;
	mutable Vector3f m_worldOrigin;
	mutable Quaternion m_worldOrientation;
};