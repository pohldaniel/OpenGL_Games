#pragma once
#include <list>
#include <vector>
#include <memory>
#include <engine/Vector.h>
#include <engine/Sprite.h>
#include <engine/utils/StringHash.h>

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

class NodeMH {

public:

	NodeMH();
	NodeMH(const NodeMH& rhs);
	NodeMH(NodeMH&& rhs);
	NodeMH& operator=(const NodeMH& rhs);
	NodeMH& operator=(NodeMH&& rhs);
	virtual ~NodeMH();

	std::list<std::shared_ptr<NodeMH>>& getChildren() const;
	void eraseChild(NodeMH* child);
	void eraseChild(const int index);
	NodeMH* addChild(NodeMH* node);
	template <class T> T* addChild();
	template <class T, class U> T* addChild(const U& ref);
	NodeMH* getParent() const;
	void setParent(NodeMH* node);
	const int getIndex() const;
	void setName(const std::string& name);
	void setIndex(const int index);
	template <class T> T* findChild(std::string name, bool recursive = true) const;
	template <class T> T* findChild(StringHash nameHash, bool recursive = true) const;
	template <class T> T* findChild(const int index, bool recursive = true) const;

protected:

	mutable std::list<std::shared_ptr<NodeMH>> m_children;
	NodeMH* m_parent;
	StringHash m_nameHash;
	int m_index;
};

template <class T> T* NodeMH::addChild() {
	m_children.emplace_back(std::shared_ptr<NodeMH>(new T()));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T, class U> T* NodeMH::addChild(const U& ref) {	
	m_children.emplace_back(std::unique_ptr<T>(new T(ref)));
	m_children.back()->m_parent = this;
	return static_cast<T*>(m_children.back().get());
}

template <class T> T* NodeMH::findChild(std::string name, bool recursive) const {
	return findChild<T>(StringHash(name), recursive);
}

template <class T> T* NodeMH::findChild(StringHash nameHash, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		NodeMH* child = (*it).get();
		if (!child) {
			continue;
		}

		if (child->m_nameHash == nameHash && dynamic_cast<T*>(child) != nullptr)
			return dynamic_cast<T*>(child);
		else if (recursive && child->m_children.size()) {
			NodeMH* result = child->findChild<T>(nameHash, recursive);
			if (result)
				return dynamic_cast<T*>(result);
		}
	}
	return nullptr;
}

template <class T> T* NodeMH::findChild(const int index, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		NodeMH* child = (*it).get();
		if (!child) {
			continue;
		}

		if (child->m_index == index && dynamic_cast<T*>(child) != nullptr)
			return dynamic_cast<T*>(child);
		else if (recursive && child->m_children.size()) {
			NodeMH* result = child->findChild<T>(index, recursive);
			if (result)
				return dynamic_cast<T*>(result);
		}
	}
	return nullptr;
}

class WidgetMH : public NodeMH, public Sprite {

public:
	
	WidgetMH();
	WidgetMH(const WidgetMH& rhs);
	WidgetMH& operator=(const WidgetMH& rhs);
	WidgetMH(WidgetMH&& rhs);
	WidgetMH& operator=(WidgetMH&& rhs);
	virtual ~WidgetMH();

	virtual void draw();
	//virtual void processInput();

	void setScale(const float sx, const float sy, const float sz) override;
	void setScale(const Vector3f& scale) override;
	void setScale(const float s) override;

	void setPosition(const float x, const float y, const float z) override;
	void setPosition(const Vector3f& position) override;

	void setOrigin(const float x, const float y, const float z) override;
	void setOrigin(const Vector3f& origin);

	void setOrientation(const Vector3f& axis, float degrees) override;
	void setOrientation(const float degreesX, const float degreesY, const float degreesZ) override;
	void setOrientation(const Vector3f& euler) override;
	void setOrientation(const Quaternion& orientation) override;
	void setOrientation(const float x, const float y, const float z, const float w) override;

	void translate(const Vector3f& trans) override;
	void translate(const float dx, const float dy, const float dz) override;

	void translateRelative(const Vector3f& trans) override;
	void translateRelative(const float dx, const float dy, const float dz) override;

	void scale(const Vector3f& scale) override;
	void scale(const float sx, const float sy, const float sz) override;
	void scale(const float s) override;

	void scaleAbsolute(const Vector3f& scale);
	void scaleAbsolute(const float sx, const float sy, const float sz);
	void scaleAbsolute(const float s);

	void rotate(const float pitch, const float yaw, const float roll) override;
	void rotate(const Vector3f& eulerAngle) override;
	void rotate(const Vector3f& axis, float degrees) override;
	void rotate(const Quaternion& orientation) override;
	void rotate(const float x, const float y, const float z, const float w) override;

	const Matrix4f& getWorldTransformation() const;
	const Vector3f& getWorldPosition(bool update = true) const;
	const Vector3f& getWorldScale(bool update = true) const;
	const Quaternion& getWorldOrientation(bool update = true) const;
	void updateWorldTransformation() const;

	const Matrix4f getWorldTransformationWithTranslation(const Vector3f& trans) const;
	const Matrix4f getWorldTransformationWithTranslation(float dx, float dy, float dz) const;

	const Matrix4f getWorldTransformationWithScale(float sx, float sy, float sz, bool relative = true) const;
	const Matrix4f getWorldTransformationWithScale(const Vector3f& scale, bool relative = true) const;
	const Matrix4f getWorldTransformationWithScale(const float s, bool relative = true) const;

	const Matrix4f getWorldTransformationWithScaleAndTranslation(float sx, float sy, float sz, float dx, float dy, float dz, bool relative = true) const;
	const Matrix4f getWorldTransformationWithScaleAndTranslation(const Vector3f& scale, const Vector3f& trans, bool relative = true) const;

protected:

	void OnTransformChanged();

private:

	mutable Matrix4f m_modelMatrix;
	static Vector3f WorldPosition;
	static Vector3f WorldScale;
	static Quaternion WorldOrientation;
	mutable bool m_isDirty;
};

