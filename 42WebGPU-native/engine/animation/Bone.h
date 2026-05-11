#pragma once
#include <list>
#include <memory>
#include <string>

#include <engine/Vector.h>

class Bone  {

	friend class AnimatedMesh;
	friend class AnimatedModel;
	friend class AnimationState;

public:

	Bone();
	~Bone();
	
	void countChildBones();

	bool animationEnabled() const;
	size_t getNumChildBones() const;

	void setPosition(const Vector3f& position);
	void setOrientation(const Quaternion& orientation);
	void setScale(const Vector3f& scale);
	void setName(const std::string& name);
	void setParent(Bone* node);
	void setIsRootBone(bool rootBone);
	void setAnimationEnabled(bool enable);
	void setTransformSilent(const Vector3f& position, const Quaternion& rotation, const Vector3f& scale);
	const bool isRootBone() const;
	void rotate(const float pitch, const float yaw, const float roll);

protected:

	void OnTransformChanged();
	const Matrix4f& getWorldTransformation() const;
	const Matrix4f& getTransformationSOP() const;

	const std::string& getName() const;
	const Vector3f& getScale() const;

private:
	
	Bone* addChild(Bone* node);
	void eraseChild(Bone* child);
	void eraseAllChildren(size_t offset = 0u);
	Bone* findChild(const std::string& name, bool recursive) const;

	std::list<std::unique_ptr<Bone>> m_children;
	Bone* m_parent;
	size_t m_numChildBones;
	bool m_isRootBone;
	mutable bool m_isDirty;
	bool m_animationEnabled;

	std::string m_name;
	Vector3f m_position;
	Vector3f m_scale;
	Quaternion m_orientation;
	Matrix4f m_offsetMatrix;

	mutable Matrix4f m_modelMatrix;
	static thread_local Matrix4f Transformation;
};