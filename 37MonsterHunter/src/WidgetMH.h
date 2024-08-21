#pragma once
#include <list>
#include <vector>
#include <memory>
#include <engine/Vector.h>

class ObjectMH {

public:

	ObjectMH();
	ObjectMH(ObjectMH const& rhs);
	ObjectMH& operator=(const ObjectMH& rhs);
	ObjectMH(ObjectMH&& rhs);
	ObjectMH& operator=(ObjectMH&& rhs);
	virtual ~ObjectMH();

	virtual void setScale(const float sx, const float sy);
	virtual void setScale(const Vector2f& scale);
	virtual void setScale(const float s);

	virtual void setPosition(const float x, const float y);
	virtual void setPosition(const Vector2f& position);

	virtual void setOrientation(const float degrees);

	virtual void translate(const Vector2f& trans);
	virtual void translate(const float dx, const float dy);

	virtual void translateRelative(const Vector2f& trans);
	virtual void translateRelative(const float dx, const float dy);

	virtual void scale(const Vector2f& scale);
	virtual void scale(const float sx, const float sy);
	virtual void scale(const float s);

	virtual void rotate(const float degrees);

	const Vector2f& getPosition() const;
	const Vector2f& getScale() const;
	const float getOrientation() const;

protected:

	Vector2f m_position;
	Vector2f m_scale;
	float m_orientation;
};

template<class T>
class NodeMH {

public:

	NodeMH() : m_parent(nullptr) {

	}

	NodeMH(const NodeMH& rhs) {
		m_children = rhs.m_children;		
		m_parent = rhs.m_parent;
	}

	NodeMH(NodeMH&& rhs) {
		m_children = rhs.m_children;		
		m_parent = rhs.m_parent;
	}

	NodeMH& operator=(const NodeMH& rhs) {
		m_children = rhs.m_children;		
		m_parent = rhs.m_parent;
		return *this;
	}

	NodeMH& operator=(NodeMH&& rhs) {
		m_children = rhs.m_children;		
		m_parent = rhs.m_parent;
		return *this;
	}

	virtual ~NodeMH() {

	}

	std::list<std::shared_ptr<T>>& getChildren() const;
	void eraseChild(T* child);
	T* addChild(T* node);
	const T* getParent() const;
	void setParent(T* node);

protected:

	mutable std::list<std::shared_ptr<T>> m_children;
	T* m_parent;
};

template<typename T>
std::list<std::shared_ptr<T>>& NodeMH<T>::getChildren() const {
	return m_children;
}

template<typename T>
void NodeMH<T>::eraseChild(T* child) {
	if (!child || child->m_parent != this)
		return;

	child->m_parent = nullptr;
	m_children.erase(std::remove_if(m_children.begin(), m_children.end(), [child](const std::shared_ptr<T>& node) { return node.get() == child; }), m_children.end());
}

template<typename T>
T* NodeMH<T>::addChild(T* node) {
	m_children.emplace_back(std::shared_ptr<NodeMH>(node));
	m_children.back()->m_parent = this;
	return m_children.back().get();
}

template<typename T>
const T* NodeMH<T>::getParent() const {
	return m_parent;
}

template<typename T>
void NodeMH<T>::setParent(T* node) {

	if (node && m_parent) {
		std::list<std::shared_ptr<T>>::iterator it = std::find_if(m_parent->getChildren().begin(), m_parent->getChildren().end(), [node](std::shared_ptr<T>& _node) { return _node.get() == node; });

		if (it != m_parent->getChildren().end()) {
			node->getChildren().splice(m_parent->getChildren().end(), m_parent->getChildren(), it);
		}
	}else if (node) {
		node->addChild(this);
	}else if (m_parent) {
		m_parent->eraseChild(this);
	}
}

class WidgetMH : public NodeMH<WidgetMH>, public ObjectMH {

public:
	
	WidgetMH();
	WidgetMH(const WidgetMH& rhs);
	WidgetMH& operator=(const WidgetMH& rhs);
	WidgetMH(WidgetMH&& rhs);
	WidgetMH& operator=(WidgetMH&& rhs);
	virtual ~WidgetMH();

	virtual void draw();
	virtual void processInput();

	const Vector2f& getWorldPosition(bool update = true) const;
	const Vector2f& getWorldScale(bool update = true) const;
	const float getWorldOrientation(bool update = true) const;
	void updateSOP() const;

protected:

	void OnTransformChanged();

	mutable Vector2f m_worldPosition;
	mutable Vector2f m_worldScale;
	mutable float m_worldOrientation;
	mutable bool m_isDirty;
};