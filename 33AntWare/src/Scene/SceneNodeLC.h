#pragma once

#include "BaseNode.h"

class SceneNodeLC : public BaseNode {

public:

	SceneNodeLC();
	const Matrix4f& getWorldTransformation() const override;

private:

	mutable Matrix4f m_modelMatrix;
};