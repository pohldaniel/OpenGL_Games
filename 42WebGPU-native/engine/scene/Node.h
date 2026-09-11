#pragma once

#include <list>
#include <functional>
#include <algorithm>
#include <memory>

class Node {

public:

	using NodeCallback = std::function<void(Node*)>;

	Node();
	Node(const Node& rhs);
	Node& operator=(const Node& rhs);
	Node(Node&& rhs);
	Node& operator=(Node&& rhs);
	virtual ~Node();

	void markForRemove();
	void setParent(Node* node);
	
	void setName(const std::string& name);
	void setId(const int id);
	
	std::list<std::unique_ptr<Node, std::function<void(Node* node)>>>& getChildren() const;
	template <typename T> std::vector<T*> getChildren() const;

	const int getId() const;
	const Node* getParent() const;

	void removeChild(Node* child);
	void removeSelf();

	void eraseSelf();
	void eraseChild(Node* child);
	void eraseChild(const int id);
	void eraseAllChildren(size_t offset = 0);
	template <class T> void eraseChildren() const;

	Node* addChild(Node* node, bool disableDelete = false);
	template <class T> T* addChild(bool disableDelete = false);
	template <class T, class U> T* addChild(const U& ref, bool disableDelete = false);
	template <class T, class U> T* addChild(U* ref, bool disableDelete = false);

	template <class T> T* findChild(std::string name, bool recursive = true) const;
	template <class T> T* findChild(const int id, bool recursive = true) const;

	template <class T> size_t countChild(bool recursive = true) const;

	size_t countNodes();

	void setOnChildAdded(NodeCallback callback);
	void setOnChildRemoved(NodeCallback callback);

	Node* attachChild(std::unique_ptr<Node, std::function<void(Node* node)>> child);
	std::unique_ptr<Node, std::function<void(Node*)>> detachChild(Node* childToDetach);

protected:

	mutable std::list<std::unique_ptr<Node, std::function<void(Node* node)>>> m_children;
	Node* m_parent;
	bool m_markForRemove;
	int m_id;
	std::string m_name;

	NodeCallback OnChildAdded;
	NodeCallback OnChildRemoved;
};

template <class T> T* Node::addChild(bool disableDelete) {

	auto child = disableDelete ? std::unique_ptr<T, std::function<void(Node* node)>>(new T(), [&](Node* node) {})
                               : std::unique_ptr<T, std::function<void(Node* node)>>(new T(), [&](Node* node) {delete node; });

	return static_cast<T*>(attachChild(std::move(child)));
}

template <class T, class U> T* Node::addChild(const U& ref, bool disableDelete) {
	
	auto child = disableDelete ? std::unique_ptr<T, std::function<void(Node* node)>>(new T(ref), [&](Node* node) {})
                               : std::unique_ptr<Node, std::function<void(Node* node)>>(new T(ref), [&](Node* node) { delete node; });

	return static_cast<T*>(attachChild(std::move(child)));
}

template <class T, class U> T* Node::addChild(U* ref, bool disableDelete) {

	auto child = disableDelete ? std::unique_ptr<T, std::function<void(Node* node)>>(new T(ref), [&](Node* node) {})
                               : std::unique_ptr<Node, std::function<void(Node* node)>>(new T(ref), [&](Node* node) { delete node; });

	return static_cast<T*>(attachChild(std::move(child)));
}

template <class T> T* Node::findChild(std::string name, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		Node* child = (*it).get();
		if (!child) {
			continue;
		}

		if (child->m_name == name && dynamic_cast<T*>(child) != nullptr)
			return dynamic_cast<T*>(child);
		else if (recursive && child->m_children.size()) {
			Node* result = child->findChild<T>(name, recursive);
			if (result)
				return dynamic_cast<T*>(result);
		}
	}
	return nullptr;
}

template <class T> T* Node::findChild(const int id, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		Node* child = (*it).get();
		if (!child) {
			continue;
		}

		if (child->m_id == id && dynamic_cast<T*>(child) != nullptr)
			return dynamic_cast<T*>(child);
		else if (recursive && child->m_children.size()) {
			Node* result = child->findChild<T>(id, recursive);
			if (result)
				return dynamic_cast<T*>(result);
		}
	}
	return nullptr;
}

template <class T> size_t Node::countChild(bool recursive) const {
	size_t num = 0;
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		Node* child = (*it).get();
		
		if (dynamic_cast<T*>(child) != nullptr)
			num++;
		if (recursive && child->m_children.size())
			num += child->countChild<T>(recursive);
	}
	return num;
}

template <class T> void Node::eraseChildren() const {

	for (auto it = m_children.begin(); it != m_children.end();) {
		Node* child = (*it).get();
		if (child && dynamic_cast<T*>(child)) {
			it = m_children.erase(it);
		}
		else ++it;
	}
}

template <typename T>
std::vector<T*> Node::getChildren() const {
	std::vector<T*> castedChildren;
	for (const auto& child : m_children) {
		if (auto* castedChild = dynamic_cast<T*>(child.get())) {
			castedChildren.push_back(castedChild);
		}
	}
	return castedChildren;
}