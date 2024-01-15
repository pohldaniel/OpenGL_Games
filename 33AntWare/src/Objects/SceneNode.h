#pragma once

#include <list>
#include <memory>
#include "Object.h"

class SceneNode : public Object {

public:
	SceneNode();

	void computeTransformation();
	void computeTransformation(const Matrix4f& parentGlobalModelMatrix);

	const Matrix4f& getTransformation();

	void forceUpdateSelfAndChild();
	void updateSelfAndChild();
	void removeChild(std::unique_ptr<SceneNode> node);
	template<typename... TArgs> void addChild(TArgs&... args);
	const std::list<std::unique_ptr<SceneNode>>& getChildren() const;
	void markForRemove();

protected:

	std::list<std::unique_ptr<SceneNode>> m_children;
	SceneNode* m_parent;	Matrix4f m_modelMatrix;	bool m_markForRemove;};template<typename... TArgs>
void SceneNode::addChild(TArgs&... args){
	m_children.emplace_back(std::make_unique<SceneNode>(args...));
	m_children.back()->m_parent = this;
}