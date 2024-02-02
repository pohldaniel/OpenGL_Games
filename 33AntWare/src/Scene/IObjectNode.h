#pragma once

#include <list>
#include <memory>
#include "Object.h"

template <class T>
class IObjectNode : public Object {

public:

	IObjectNode() : Object(), m_parent(nullptr), m_markForRemove(false), m_isDirty(true) {

	}

	IObjectNode(const IObjectNode& rhs) : Object(rhs) {}
	IObjectNode& operator=(const IObjectNode& rhs) {
		Object::operator=(rhs);
		return *this;
	}

	const std::list<std::unique_ptr<T>>& getChildren() const {
		return m_children;
	}

	void removeChild(std::unique_ptr<T> node) {
		m_children.remove(node);
	}

	void addChild(T* node, T* parent) {
		m_children.emplace_back(std::unique_ptr<T>(node));
		m_children.back()->m_parent = parent;
	}

	void addChild(T* parent) {
		m_children.emplace_back(std::make_unique<T>());
		m_children.back()->m_parent = parent;
	}

	void setParent(T* node) {
		m_parent = node;
	}

	void markForRemove() {
		m_markForRemove = true;
	}

	virtual void OnTransformChanged() {

		if (m_isDirty) {
			return;
		}

		for (auto&& child : m_children) {
			child->OnTransformChanged();
		}

		m_isDirty = true;
	}

	void setScale(const float sx, const float sy, const float sz) override {
		Object::setScale(sx, sy, sz);
		OnTransformChanged();
	}

	void setScale(const Vector3f& scale) override {
		Object::setScale(scale);
		OnTransformChanged();
	}

	void setScale(const float s) override {
		Object::setScale(s);
		OnTransformChanged();
	}

	void setPosition(const float x, const float y, const float z) override {
		Object::setPosition(x, y, z);
		OnTransformChanged();
	}

	void setPosition(const Vector3f& position) override {
		Object::setPosition(position);
		OnTransformChanged();
	}

	void setOrientation(const Vector3f& axis, float degrees) override {
		Object::setOrientation(axis, degrees);
		OnTransformChanged();
	}

	void setOrientation(const float degreesX, const float degreesY, const float degreesZ) override {
		Object::setOrientation(degreesX, degreesY, degreesZ);
		OnTransformChanged();
	}

	void setOrientation(const Vector3f& euler) override {
		Object::setOrientation(euler);
		OnTransformChanged();
	}

	void setOrientation(const Quaternion& orientation) override {
		Object::setOrientation(orientation);
		OnTransformChanged();
	}

	void translate(const Vector3f& trans) override {
		Object::translate(trans);
		OnTransformChanged();
	}

	void translate(const float dx, const float dy, const float dz) override {
		Object::translate(dx, dy, dz);
		OnTransformChanged();
	}

	void translateRelative(const Vector3f& trans) override {
		Object::translateRelative(trans);
		OnTransformChanged();
	}

	void translateRelative(const float dx, const float dy, const float dz) override {
		Object::translateRelative(dx, dy, dz);
		OnTransformChanged();
	}

	void scale(const Vector3f& scale) override {
		Object::scale(scale);
		OnTransformChanged();
	}

	void scale(const float sx, const float sy, const float sz) override {
		Object::scale(sx, sy, sz);
		OnTransformChanged();
	}

	void scale(const float s) override {
		Object::scale(s);
		OnTransformChanged();
	}

	void rotate(const float pitch, const float yaw, const float roll) override {
		Object::rotate(pitch, yaw, roll);
		OnTransformChanged();
	}

	void rotate(const Vector3f& eulerAngle) override {
		Object::rotate(eulerAngle);
		OnTransformChanged();
	}

	void rotate(const Vector3f& axis, float degrees) override {
		Object::rotate(axis, degrees);
		OnTransformChanged();
	}

	void rotate(const Quaternion& orientation) override {
		Object::rotate(orientation);
		OnTransformChanged();
	}

	virtual const Matrix4f& getTransformation() const {
		return getTransformationSOP();
	}

protected:

	T* m_parent;	bool m_markForRemove;	mutable bool m_isDirty;	std::list<std::unique_ptr<T>> m_children;};