#pragma once

#include <list>
#include <functional>
#include <memory>
#include "../Object.h"
#include "Node.h"

class BaseNode : public Node, public Object {

	friend class SceneNode;

public:

	BaseNode();
	BaseNode(const BaseNode& rhs);
	BaseNode& operator=(const BaseNode& rhs);
	BaseNode(BaseNode&& rhs);
	BaseNode& operator=(BaseNode&& rhs);

	virtual const Matrix4f& getWorldTransformation() const = 0;
	virtual const Vector3f& getWorldPosition(bool update = true) const = 0;
	virtual const Vector3f& getWorldScale(bool update = true) const = 0;
	virtual const Quaternion& getWorldOrientation(bool update = true) const = 0;

	void setScale(const float sx, const float sy, const float sz) const override;
	void setScale(const Vector3f& scale) const override;
	void setScale(const float s) const override;

	void setPosition(const float x, const float y, const float z) const override;
	void setPosition(const Vector3f& position) const override;

	void setOrientation(const Vector3f& axis, float degrees) const override;
	void setOrientation(const float degreesX, const float degreesY, const float degreesZ) const override;
	void setOrientation(const Vector3f& euler) const override;
	void setOrientation(const Quaternion& orientation) const override;
	void setOrientation(const float x, const float y, const float z, const float w) const override;

	void translate(const Vector3f& trans) override;
	void translate(const float dx, const float dy, const float dz) override;

	void translateRelative(const Vector3f& trans) override;
	void translateRelative(const float dx, const float dy, const float dz) override;

	void scale(const Vector3f& scale) override;
	void scale(const float sx, const float sy, const float sz) override;
	void scale(const float s) override;

	void rotate(const float pitch, const float yaw, const float roll) override;
	void rotate(const Vector3f& eulerAngle) override;
	void rotate(const Vector3f& axis, float degrees) override;
	void rotate(const Quaternion& orientation) override;
	void rotate(const float x, const float y, const float z, const float w) override;

protected:

	virtual void OnTransformChanged() const;
	virtual const Vector3f& getWorldOrigin(bool update = true) const;	mutable bool m_isDirty;
};