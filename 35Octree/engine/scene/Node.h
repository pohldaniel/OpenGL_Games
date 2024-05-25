#pragma once

#include <list>
#include <functional>
#include <algorithm>
#include <memory>
#include "../Camera.h"
class Node {

public:

	Node();
	Node(const Node& rhs);
	Node& operator=(const Node& rhs);
	Node(Node&& rhs);
	Node& operator=(Node&& rhs);
	virtual ~Node();

	
	void markForRemove();
	void setParent(Node* node);
	
	std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& getChildren() const;
	void removeAllChildren();
	void removeChild(Node* child);
	void removeSelf();

	void eraseChild(Node* child);
	void eraseSelf();

	Node* addChild(Node* node, bool disableDelete = false);

	template <class T> T* addChild(bool disableDelete = false);
	template <class T, class U> T* addChild(const U& ref, bool disableDelete = false);
	template <class T, class U> T* addChild(const U& ref, const Camera& camera, bool disableDelete = false);
	const Node* getParent() const;

protected:

	mutable std::list<std::unique_ptr<Node, std::function<void(Node* node)>>> m_children;
	Node* m_parent;
	bool m_markForRemove;
};

template <class T> T* Node::addChild(bool disableDelete) {
	if (disableDelete)
		m_children.emplace_back(std::unique_ptr<T, std::function<void(Node* node)>>(new T(), [&](Node* node) {}));
	else
		m_children.emplace_back(std::unique_ptr<T, std::function<void(Node* node)>>(new T(), [&](Node* node) {delete node; }));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T, class U> T* Node::addChild(const U& ref, bool disableDelete) {
	if (disableDelete)
		m_children.emplace_back(std::unique_ptr<T, std::function<void(Node* node)>>(new T(ref), [&](Node* node) {}));
	else
		m_children.emplace_back(std::unique_ptr<T, std::function<void(Node* node)>>(new T(ref), [&](Node* node) {delete node; }));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T, class U> T* Node::addChild(const U& ref, const Camera& camera, bool disableDelete) {
	if (disableDelete)
		m_children.emplace_back(std::unique_ptr<T, std::function<void(Node* node)>>(new T(ref, camera), [&](Node* node) {}));
	else
		m_children.emplace_back(std::unique_ptr<T, std::function<void(Node* node)>>(new T(ref, camera), [&](Node* node) {delete node; }));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}