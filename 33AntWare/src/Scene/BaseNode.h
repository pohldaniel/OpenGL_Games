#pragma once
#include <list>
#include <memory>
#include "Object.h"

class BaseNode : public Object {

	friend class SceneNode;

public:

	BaseNode();
	BaseNode(const BaseNode& rhs);
	BaseNode& operator=(const BaseNode& rhs);
	BaseNode(BaseNode&& rhs);
	BaseNode& operator=(BaseNode&& rhs);

	virtual const Matrix4f& getWorldTransformation() const = 0;
	virtual void OnTransformChanged();

	void setScale(const float sx, const float sy, const float sz) override;
	void setScale(const Vector3f& scale) override;
	void setScale(const float s) override;

	void setPosition(const float x, const float y, const float z) override;
	void setPosition(const Vector3f& position) override;

	void setOrientation(const Vector3f& axis, float degrees) override;
	void setOrientation(const float degreesX, const float degreesY, const float degreesZ) override;
	void setOrientation(const Vector3f& euler) override;
	void setOrientation(const Quaternion& orientation) override;

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

	void markForRemove();
	void setIsFixed(bool isFixed);
	void setParent(BaseNode* node);
	const bool isFixed() const;


	const std::list<std::unique_ptr<BaseNode>>& getChildren() const;
	void removeChild(std::unique_ptr<BaseNode> node);
	BaseNode* addChild(BaseNode* node);
	BaseNode* addChild();
	
protected:

	virtual const Vector3f& getWorldPosition() const = 0;	
	virtual const Vector3f& getWorldScale() const = 0;
	virtual const Quaternion& getWorldOrientation() const = 0;
	virtual const Vector3f& getWorldOrigin() const;

	BaseNode* m_parent;	std::list<std::unique_ptr<BaseNode>> m_children;
	bool m_markForRemove;	bool m_isFixed;	mutable bool m_isDirty;
};