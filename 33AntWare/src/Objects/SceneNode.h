#pragma once

#include <list>
#include <memory>
#include <vector>

#include "Object.h"

class Entity;
class SceneNode : public Object {

public:
	SceneNode();

	void computeTransformation();
	void computeTransformation(const Matrix4f& parentGlobalModelMatrix);

	const Matrix4f& getTransformation();

	void forceUpdateSelfAndChild();
	void updateSelfAndChild();
	void removeChild(std::unique_ptr<SceneNode> node);
	void addChild(SceneNode* node);
	void addChild();

	const std::list<std::unique_ptr<SceneNode>>& getChildren() const;
	void markForRemove();

protected:

	std::list<std::unique_ptr<SceneNode>> m_children;
	SceneNode* m_parent;	Matrix4f m_modelMatrix;	bool m_markForRemove;};