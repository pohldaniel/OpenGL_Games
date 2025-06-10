#pragma once

#include "BaseNode.h"
#include "../utils/StringHash.h"

class SceneNodeLC : public BaseNode {

public:

	SceneNodeLC();
	SceneNodeLC(const SceneNodeLC& rhs);
	SceneNodeLC& operator=(const SceneNodeLC& rhs);
	SceneNodeLC(SceneNodeLC&& rhs);
	SceneNodeLC& operator=(SceneNodeLC&& rhs);

	const Matrix4f& getWorldTransformation() const override;
	const Vector3f& getWorldPosition(bool update = true) const override;
	const Vector3f& getWorldScale(bool update = true) const override;
	const Quaternion& getWorldOrientation(bool update = true) const override;
	void updateWorldTransformation() const;

	void setName(const std::string& name);
	template <class T> T* findChild(std::string name, bool recursive = true) const;
	template <class T> T* findChild(StringHash nameHash, bool recursive = true) const;

protected:

	StringHash m_nameHash;

private:

	mutable Matrix4f m_modelMatrix;
	static thread_local Vector3f WorldPosition;
	static thread_local Vector3f WorldScale;
	static thread_local Quaternion WorldOrientation;
};

template <class T> T* SceneNodeLC::findChild(std::string name, bool recursive) const {
	return findChild<T>(StringHash(name), recursive);
}

template <class T> T* SceneNodeLC::findChild(StringHash nameHash, bool recursive) const {
	for (auto it = m_children.begin(); it != m_children.end(); ++it) {
		SceneNodeLC* child = dynamic_cast<SceneNodeLC*>((*it).get());
		if (!child) {
			continue;
		}

		if (child->m_nameHash == nameHash)
			return static_cast<T*>(child);
		else if (recursive && child->m_children.size()) {
			SceneNodeLC* result = child->findChild<T>(nameHash, recursive);
			if (result)
				return static_cast<T*>(result);
		}
	}
	return nullptr;
}