#pragma once

#include "BaseNode.h"

class SceneNode : public BaseNode {

public:

	SceneNode();
	const Matrix4f& getTransformation() const override;

private:

	mutable Matrix4f m_modelMatrix;
};