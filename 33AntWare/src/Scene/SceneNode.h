#pragma once

#include "IObjectNode.h"

class SceneNode : public IObjectNode<SceneNode>{

public:

	SceneNode();
	const Matrix4f& getTransformation() const;

private:

	mutable Matrix4f m_modelMatrix;
};