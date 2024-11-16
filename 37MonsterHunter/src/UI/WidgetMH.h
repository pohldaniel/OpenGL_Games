#pragma once
#include <list>
#include <vector>
#include <memory>
#include <functional>
#include <engine/Vector.h>
#include <engine/Sprite.h>
#include <engine/utils/StringHash.h>

class NodeUI {

public:

	NodeUI();
	NodeUI(const NodeUI& rhs);
	NodeUI(NodeUI&& rhs);
	virtual ~NodeUI();

	std::list<std::shared_ptr<NodeUI>>& getChildren() const;
	void eraseSelf();
	void eraseChild(NodeUI* child);
	void eraseChild(const int index);
	void eraseAllChildren(size_t offset = 0);
	template <class T> void eraseChildren() const;
	NodeUI* addChild(NodeUI* node);
	template <class T> T* addChild();
	template <class T, class U> T* addChild(const U& ref);
	template <class T, class U> T* addChild(U& ref);
	template <class T, class U, class V> T* addChild(const U& ref1, const V& ref2);
	template <class T, class U, class V> T* addChild(U& ref1, V& ref2);
	NodeUI* getParent() const;
	void setParent(NodeUI* node);
	const int getIndex() const;
	void setName(const std::string& name);
	void setIndex(const int index);
	template <class T> T* findChild(std::string name, bool recursive = true) const;
	template <class T> T* findChild(StringHash nameHash, bool recursive = true) const;
	template <class T> T* findChild(const int index, bool recursive = true) const;
	size_t countNodes();

protected:

	mutable std::list<std::shared_ptr<NodeUI>> m_children;
	NodeUI* m_parent;
	StringHash m_nameHash;
	int m_index;
};

template <class T> T* NodeUI::addChild() {
	m_children.emplace_back(std::shared_ptr<NodeUI>(new T()));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T, class U> T* NodeUI::addChild(const U& ref) {
	m_children.emplace_back(std::unique_ptr<T>(new T(ref)));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T, class U> T* NodeUI::addChild(U& ref) {
	m_children.emplace_back(std::unique_ptr<T>(new T(ref)));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T, class U, class V> T* NodeUI::addChild(const U& ref1, const V& ref2) {
	m_children.emplace_back(std::unique_ptr<T>(new T(ref1, ref2)));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T, class U, class V> T* NodeUI::addChild(U& ref1, V& ref2) {
	m_children.emplace_back(std::unique_ptr<T>(new T(ref1, ref2)));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T> T* NodeUI::findChild(std::string name, bool recursive) const {
	return findChild<T>(StringHash(name), recursive);
}

template <class T> T* NodeUI::findChild(StringHash nameHash, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		NodeUI* child = (*it).get();
		if (!child) {
			continue;
		}

		if (child->m_nameHash == nameHash && dynamic_cast<T*>(child) != nullptr)
			return dynamic_cast<T*>(child);
		else if (recursive && child->m_children.size()) {
			NodeUI* result = child->findChild<T>(nameHash, recursive);
			if (result)
				return dynamic_cast<T*>(result);
		}
	}
	return nullptr;
}

template <class T> T* NodeUI::findChild(const int index, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		NodeUI* child = (*it).get();
		if (!child) {
			continue;
		}

		if (child->m_index == index && dynamic_cast<T*>(child) != nullptr)
			return dynamic_cast<T*>(child);
		else if (recursive && child->m_children.size()) {
			NodeUI* result = child->findChild<T>(index, recursive);
			if (result)
				return dynamic_cast<T*>(result);
		}
	}
	return nullptr;
}

template <class T> void NodeUI::eraseChildren() const {

	for (auto it = m_children.begin(); it != m_children.end();){
		NodeUI* child = (*it).get();
		if (child && dynamic_cast<T*>(child)){
			it = m_children.erase(it);
		}
		else ++it;
	}
}

class WidgetMH : public NodeUI, public Sprite {

public:
	
	WidgetMH();
	WidgetMH(const WidgetMH& rhs);
	WidgetMH(WidgetMH&& rhs);
	virtual ~WidgetMH();

	virtual void draw();

	void setScale(const float sx, const float sy) override;
	void setScale(const Vector2f& scale) override;
	void setScale(const float s) override;

	void setScaleAbsolute(const float sx, const float sy);
	void setScaleAbsolute(const Vector2f& scale);
	void setScaleAbsolute(const float s);

	void setPosition(const float x, const float y) override;
	void setPosition(const Vector2f& position) override;

	void setOrigin(const float x, const float y) override;
	void setOrigin(const Vector2f& origin);

	void setOrientation(const float degrees) override;

	void translate(const Vector2f& trans) override;
	void translate(const float dx, const float dy) override;

	void translateRelative(const Vector2f& trans) override;
	void translateRelative(const float dx, const float dy) override;

	void scale(const Vector2f& scale) override;
	void scale(const float sx, const float sy) override;
	void scale(const float s) override;

	void scaleAbsolute(const Vector2f& scale);
	void scaleAbsolute(const float sx, const float sy);
	void scaleAbsolute(const float s);

	void rotate(const float degrees) override;

	const Matrix4f& getWorldTransformation() const;
	const Vector2f& getWorldPosition(bool update = true) const;
	const Vector2f& getWorldScale(bool update = true) const;
	const float getWorldOrientation(bool update = true) const;
	void updateWorldTransformation() const;

	const Matrix4f getWorldTransformationWithTranslation(const Vector2f& trans) const;
	const Matrix4f getWorldTransformationWithTranslation(float dx, float dy) const;

	const Matrix4f getWorldTransformationWithScale(float sx, float sy, bool relative = true) const;
	const Matrix4f getWorldTransformationWithScale(const Vector2f& scale, bool relative = true) const;
	const Matrix4f getWorldTransformationWithScale(const float s, bool relative = true) const;

	const Matrix4f getWorldTransformationWithScaleAndTranslation(float sx, float sy, float dx, float dy, bool relative = true) const;
	const Matrix4f getWorldTransformationWithScaleAndTranslation(const Vector2f& scale, const Vector2f& trans, bool relative = true) const;
	void setDrawFunction(std::function<void()> fun);

protected:
	
	void OnTransformChanged();
	std::function<void()> m_draw;

private:

	virtual void drawDefault() = 0;

	mutable Matrix4f m_modelMatrix;
	static Vector2f WorldPosition;
	static Vector2f WorldScale;
	static float WorldOrientation;
	mutable bool m_isDirty;
};

