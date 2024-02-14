#pragma once

#include <list>
#include <memory>
#include "Object.h"

template <class T>
class IBaseNode : public Object {

public:

	IBaseNode() : Object(), m_parent(nullptr) {

	}

	IBaseNode(const IBaseNode& rhs) : Object(rhs) {}
	IBaseNode& operator=(const IBaseNode& rhs) {
		Object::operator=(rhs);
		return *this;
	}

	IBaseNode(IBaseNode&& rhs) : Object(rhs) {}
	IBaseNode& operator=(IBaseNode&& rhs) {
		Object::operator=(rhs);
		return *this;
	}

	const std::list<std::unique_ptr<T>>& getChildren() const {
		return m_children;
	}

	void removeChild(std::unique_ptr<T> node) {
		m_children.remove(node);
	}

	T* addChild(T* node, T* parent) {
		m_children.emplace_back(std::unique_ptr<T>(node));
		m_children.back()->m_parent = parent;
		return m_children.back().get();
	}

	void setParent(T* node) {
		m_parent = node;
	}

protected:

	T* m_parent;	std::list<std::unique_ptr<T>> m_children;};