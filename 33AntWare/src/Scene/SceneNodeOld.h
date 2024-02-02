#pragma once

#include "BaseNode.h"

class SceneNodeOld : public BaseNode {

public:

	SceneNodeOld();
	const Matrix4f& getTransformation() const override;

private:

	mutable Matrix4f m_modelMatrix;
};