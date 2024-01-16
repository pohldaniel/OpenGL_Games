#include "SceneNode.h"

SceneNode::SceneNode() : Object(), m_parent(nullptr), m_markForRemove(false){
	m_modelMatrix.identity();
}

/*SceneNode::SceneNode(SceneNode const& rhs) : Object(rhs) {

}

SceneNode& SceneNode::operator=(const SceneNode& rhs) {
	Object::operator=(rhs);
	return *this;
}*/

void SceneNode::computeTransformation() {
	m_modelMatrix = getTransformationSOP();
	m_isDirty = false;
}

void SceneNode::computeTransformation(const Matrix4f& parentGlobalModelMatrix) {
	m_modelMatrix = parentGlobalModelMatrix * getTransformationSOP();
	m_isDirty = false;
}

const Matrix4f& SceneNode::getTransformation() {
	return m_modelMatrix;
}

void SceneNode::updateSelfAndChild(){
	if (isDirty()) {
		forceUpdateSelfAndChild();
		return;
	}

	for (auto&& child : m_children){
		child->updateSelfAndChild();
	}
}

void SceneNode::forceUpdateSelfAndChild(){
	if (m_parent)
		computeTransformation(m_parent->getTransformation());
	else 
		computeTransformation();
	

	for (auto&& child : m_children){
		child->forceUpdateSelfAndChild();
	}
}

void SceneNode::removeChild(std::unique_ptr<SceneNode> node){
	m_children.remove(node);
}

const std::list<std::unique_ptr<SceneNode>>& SceneNode::getChildren() const {
	return m_children;
}

void SceneNode::markForRemove() {
	m_markForRemove = true;
}

void SceneNode::addChild(SceneNode* node) {
	m_children.emplace_back(std::unique_ptr<SceneNode>(node));
	m_children.back()->m_parent = this;
}


void SceneNode::addChild() {
	m_children.emplace_back(std::make_unique<SceneNode>());
	m_children.back()->m_parent = this;
}