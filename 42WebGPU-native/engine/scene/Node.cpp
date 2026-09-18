#include "Node.h"

Node::Node() : m_parent(nullptr), m_markForRemove(false), m_id(-1), OnChildAdded(nullptr), OnChildRemoved(nullptr){

}

Node::Node(const Node& rhs) : 
	m_parent(rhs.m_parent), 
	m_markForRemove(rhs.m_markForRemove), 
	m_id(rhs.m_id),  
	m_name(rhs.m_name),
	OnChildAdded(rhs.OnChildAdded),
	OnChildRemoved(rhs.OnChildRemoved) {
	
	for (const auto& childPtr : rhs.m_children) {
		if (childPtr) {
			m_children.emplace_back(new Node(*childPtr), childPtr.get_deleter());
			m_children.back()->m_parent = this;
		}
	}
}

Node& Node::operator=(const Node& rhs) {
	m_parent = rhs.m_parent;
	m_markForRemove = rhs.m_markForRemove;
	m_id = rhs.m_id;
	m_name = rhs.m_name;
	OnChildAdded = rhs.OnChildAdded;
	OnChildRemoved = rhs.OnChildRemoved;

	for (const auto& childPtr : rhs.m_children) {
		if (childPtr) {
			m_children.emplace_back(new Node(*childPtr), childPtr.get_deleter());
			m_children.back()->m_parent = this;
		}
	}
	return *this;
}

Node::Node(Node&& rhs) noexcept :
	m_parent(rhs.m_parent),
	m_markForRemove(rhs.m_markForRemove),
	m_id(rhs.m_id),
	m_name(rhs.m_name),
	m_children(std::move(rhs.m_children)),
	OnChildAdded(std::move(rhs.OnChildAdded)),
	OnChildRemoved(std::move(rhs.OnChildRemoved)) {

	for (auto& childPtr : m_children) {
		if (childPtr) {
			childPtr->m_parent = this;
		}
	}

	rhs.m_parent = nullptr;
	rhs.m_markForRemove = false;
}

Node& Node::operator=(Node&& rhs) noexcept {
	m_parent = rhs.m_parent;
	m_markForRemove = rhs.m_markForRemove;
	m_id = rhs.m_id;
	m_name = rhs.m_name;
	m_children = std::move(rhs.m_children);
	OnChildAdded = std::move(rhs.OnChildAdded);
	OnChildRemoved = std::move(rhs.OnChildRemoved);

	for (auto& childPtr : m_children) {
		if (childPtr) {
			childPtr->m_parent = this;
		}
	}

	rhs.m_parent = nullptr;
	rhs.m_markForRemove = false;
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
	if (node->m_parent) {
		node->setParent(this);
		return node;
	}

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

void Node::setParent(Node* node, bool disableDelete) {
	if (m_parent == node) return;
	if (m_parent) {
		std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>::iterator it = std::find_if(m_parent->getChildren().begin(), m_parent->getChildren().end(), [node](std::unique_ptr<Node, std::function<void(Node* node)>>& _node) { return _node.get() == node; });
		if (it != m_parent->getChildren().end()) {			
			//node->m_children.splice(node->m_children.end(), m_parent->getChildren(), it);

			Node* rawPtr = it->release();
			m_parent->m_children.erase(it);
			if (disableDelete) 
				node->m_children.emplace_back(std::unique_ptr<Node, std::function<void(Node * node)>>(rawPtr, [&](Node* rawPtr) {}));
			else
				node->m_children.emplace_back(std::unique_ptr<Node, std::function<void(Node * node)>>(rawPtr, [&](Node* rawPtr) {delete rawPtr; }));
			
			m_parent = node;
			return;
		}else {
			m_parent->m_children.erase(it);
			m_parent = nullptr;
			return;
		}
	}

	if (!m_parent && node) {
		node->addChild(this, disableDelete);
	}
}

void Node::setName(const std::string& name) {
	m_name = name;
}

void Node::setId(const int id) {
	m_id = id;
}

void Node::eraseChild(const int id) {
	auto it = std::find_if(m_children.begin(), m_children.end(),
		[id](const auto& childPtr) {
			return childPtr && childPtr->getId() == id;
	});


	if (it != m_children.end()) {
		(*it)->m_parent = nullptr;
		m_children.erase(it);
	}
}

void Node::eraseAllChildren(size_t offset) {
	if (offset >= m_children.size())
		return;

	auto startIt = std::next(m_children.begin(), offset);

	std::for_each(startIt, m_children.end(), [](const auto& childPtr) {
		childPtr->m_parent = nullptr;
	});

	m_children.erase(startIt, m_children.end());
}

void Node::eraseChild(Node* child) {
	if (!child || child->m_parent != this)
		return;

	//out-of-scope the unique pointer will be destroyed
	detachChild(child);
}

void Node::eraseChildSilent(Node* child) {
	if (!child || child->m_parent != this)
		return;
	detachChildSilent(child);
}

void Node::eraseSelf() {
	if (m_parent)
		m_parent->eraseChild(this);
}

Node* Node::removeChild(Node* child) {
	auto ownedChild = detachChild(child);
	if (ownedChild) {
		return ownedChild.release();
	}
	return nullptr;
}

Node* Node::removeChildSilent(Node* child) {
	auto ownedChild = detachChildSilent(child);
	if (ownedChild) {
		return ownedChild.release();
	}
	return nullptr;
}

Node* Node::removeSelf() {
	if (m_parent)
		return m_parent->removeChild(this);
	return nullptr;
}

size_t Node::countNodes() {
	size_t num = m_children.size();
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		num += (*it)->countNodes();
	}

	return num;
}

void  Node::setOnChildAdded(NodeCallback callback) {
	OnChildAdded = std::move(callback);
}

void Node::setOnChildRemoved(NodeCallback callback) {
	OnChildRemoved = std::move(callback);
}

Node* Node::attachChild(std::unique_ptr<Node, std::function<void(Node*)>> child) {	
	Node* rawPtr = child.get();
	rawPtr->m_parent = this;
	m_children.push_back(std::move(child));

	if (OnChildAdded) {
		OnChildAdded(rawPtr);
	}

	return rawPtr;
}

Node* Node::attachChildSilent(std::unique_ptr<Node, std::function<void(Node* node)>> child) {
	Node* rawPtr = child.get();
	rawPtr->m_parent = this;
	m_children.push_back(std::move(child));
	return rawPtr;
}

std::unique_ptr<Node, std::function<void(Node*)>> Node::detachChild(Node* childToDetach) {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		if (it->get() == childToDetach) {
			auto ownedChild = std::move(*it);
			m_children.erase(it);
			ownedChild->m_parent = nullptr;

			if (OnChildRemoved) {
				OnChildRemoved(childToDetach);
			}

			return ownedChild;
		}
	}
	return nullptr;
}

std::unique_ptr<Node, std::function<void(Node*)>> Node::detachChildSilent(Node* childToDetach) {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		if (it->get() == childToDetach) {
			auto ownedChild = std::move(*it);
			m_children.erase(it);
			ownedChild->m_parent = nullptr;
			return ownedChild;
		}
	}
	return nullptr;
}