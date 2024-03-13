#pragma once

#include <list>
#include <memory>

template <class T>
class INode {

public:

	INode() : m_parent(nullptr) {

	}
	
	void setParent(T* node) {
		if (node && m_parent) {
			std::list<std::unique_ptr<T, std::function<void(T* node)>>>::iterator it = std::find_if(m_parent->getChildren().begin(), m_parent->getChildren().end(), [node](std::unique_ptr<T, std::function<void(T* node)>>& _node) { return _node.get() == node; });

			if (it != m_parent->getChildren().end()) {
				node->getChildren().splice(m_parent->getChildren().end(), m_parent->getChildren(), it);
			}
		}else if (node) {
			node->addChild(this->template);
		}else if (m_parent) {
			m_parent->eraseChild(this->template);
		}
	}

	std::list<std::unique_ptr<T, std::function<void(T* node)>>>& getChildren() const {
		return m_children;
	}

	void removeAllChildren() {
		for (auto it = m_children.begin(); it != m_children.end(); ++it) {
			T* child = (*it).release();
			child->m_parent = nullptr;
			delete child;
			child = nullptr;
		}
		m_children.clear();
	}

	void removeChild(T* child) {
		if (!child || child->m_parent != this->template)
			return;

		child->m_parent = nullptr;

		std::remove_if(m_children.begin(), m_children.end(), [child](const std::unique_ptr<T, std::function<void(T* node)>>& node) -> bool { return node.get() == child; });
	}

	void removeSelf() {
		if (m_parent)
			m_parent->removeChild(this->template);
	}

	void eraseChild(T* child) {
		if (!child || child->m_parent != this->template)
			return;

		child->m_parent = nullptr;
		m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child](const std::unique_ptr<T, std::function<void(T* node)>>& node) { return node.get() == child; }), m_children.end());

	}

	void eraseSelf() {
		if (m_parent)
			m_parent->eraseChild(this->template);
	}

	T* addChild(T* node, bool disableDelete = false) {
		if (disableDelete)
			m_children.emplace_back(std::unique_ptr<T, std::function<void(T* node)>>(node, [&](T* node) {}));
		else
			m_children.emplace_back(std::unique_ptr<T, std::function<void(T* node)>>(node, [&](T* node) {delete node; }));
		m_children.back()->m_parent = this->template;
		return m_children.back().get();
	}

	template <class U> U* addChild(bool disableDelete = false) {
		if (disableDelete)
			m_children.emplace_back(std::unique_ptr<U, std::function<void(T* node)>>(new T(), [&](T* node) {}));
		else
			m_children.emplace_back(std::unique_ptr<U, std::function<void(T* node)>>(new T(), [&](T* node) {delete node; }));
		m_children.back()->m_parent = this->template;
		return static_cast<T*>(m_children.back().get());
	}

	const T* getParent() const {
		return m_parent;
	}

protected:

	T* m_parent;	mutable std::list<std::unique_ptr<T, std::function<void(T* node)>>> m_children;};