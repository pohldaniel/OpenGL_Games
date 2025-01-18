#include "Node.h"

Node::Node() : m_parent(nullptr), m_markForRemove(false), m_id(-1){

}

Node::Node(const Node& rhs) {
	m_markForRemove = rhs.m_markForRemove;
}

Node& Node::operator=(const Node& rhs) {
	m_markForRemove = rhs.m_markForRemove;
	return *this;
}

Node::Node(Node&& rhs) {
	m_markForRemove = rhs.m_markForRemove;
}

Node& Node::operator=(Node&& rhs) {
	m_markForRemove = rhs.m_markForRemove;
	return *this;
}

Node::~Node() {
	eraseAllChildren();
}

void Node::markForRemove() {
	m_markForRemove = true;
}

std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& Node::getChildren() const {
	return m_children;
}

Node* Node::addChild(Node* node, bool disableDelete) {
	if (disableDelete)
		m_children.emplace_back(std::unique_ptr<Node, std::function<void(Node* node)>>(node, [&](Node* node) {}));
	else
		m_children.emplace_back(std::unique_ptr<Node, std::function<void(Node* node)>>(node, [&](Node* node) {delete node; }));
	m_children.back()->m_parent = this;
	return m_children.back().get();
}

const Node* Node::getParent() const {
	return m_parent;
}

const int Node::getId() const {
	return m_id;
}

void Node::setParent(Node* node) {

	if (node && m_parent) {
		std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = std::find_if(m_parent->getChildren().begin(), m_parent->getChildren().end(), [node](std::unique_ptr<Node, std::function<void(Node* node)>>& _node) { return _node.get() == node; });

		if (it != m_parent->getChildren().end()) {
			node->getChildren().splice(m_parent->getChildren().end(), m_parent->getChildren(), it);
		}
	}else if (node) {
		node->addChild(this);
	}else if (m_parent) {
		m_parent->eraseChild(this);
	}
}

void Node::setName(const std::string& name) {
	m_nameHash = StringHash(name);
}

void Node::setId(const int id) {
	m_id = id;
}

void Node::eraseChild(const int id) {
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [id](const std::unique_ptr<Node, std::function<void(Node* node)>>& node) { return node->getId() == id; }), m_children.end());
}

void Node::eraseAllChildren(size_t offset) {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		Node* child = (*it).release();
		child->m_parent = nullptr;
		delete child;
		child = nullptr;
	}
	//m_children.clear();
	m_children.erase(std::next(m_children.begin(), offset), m_children.end());
}

void Node::eraseChild(Node* child) {

	if (!child || child->m_parent != this)
		return;

	child->m_parent = nullptr;
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child](const std::unique_ptr<Node, std::function<void(Node* node)>>& node) { return node.get() == child; }), m_children.end());
}

void Node::eraseSelf() {
	if (m_parent)
		m_parent->eraseChild(this);
}

void Node::removeChild(Node* child) {

	if (!child || child->m_parent != this)
		return;

	child->m_parent = nullptr;

	[[maybe_unused]]auto tmp = std::remove_if(m_children.begin(), m_children.end(), [child](const std::unique_ptr<Node, std::function<void(Node* node)>>& node) -> bool { return node.get() == child; });
}

void Node::removeSelf() {
	if (m_parent)
		m_parent->removeChild(this);
}

size_t Node::countNodes() {
	size_t num = m_children.size();
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		num += (*it)->countNodes();
	}

	return num;
}