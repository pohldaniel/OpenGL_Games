#pragma once

#include <list>
#include <memory>
#include <vector>

#include "Object.h"

class SceneNode : public Object {

public:

	SceneNode();
	
	const Matrix4f& getTransformation() const;
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

	void removeChild(std::unique_ptr<SceneNode> node);
	void addChild(SceneNode* node);
	void addChild();

	const std::list<std::unique_ptr<SceneNode>>& getChildren() const;
	void markForRemove();
	void setParent(SceneNode* node);

	mutable Matrix4f m_modelMatrix;
	std::list<std::unique_ptr<SceneNode>> m_children;

private:

	SceneNode* m_parent;	bool m_markForRemove;	mutable bool m_isDirty;};