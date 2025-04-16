#pragma once

#include "../engine/scene/SceneNodeLC.h"

class Navigable {

	friend class NavigationMesh;

public:

	Navigable(SceneNodeLC* node);
	virtual ~Navigable();
	void setRecursive(bool enable);
	bool isRecursive() const;

private:

	SceneNodeLC* m_node;
	bool m_recursive;
};