#pragma once

#include "../engine/scene/SceneNodeLC.h"

class Navigable {

	friend class NavigationMesh;

public:

	Navigable(SceneNodeLC* node);
	Navigable(Navigable const& rhs);
	Navigable(Navigable&& rhs);
	Navigable& operator=(const Navigable& rhs);
	Navigable& operator=(Navigable&& rhs);
	virtual ~Navigable();
	void setRecursive(bool enable);
	bool isRecursive() const;

private:

	SceneNodeLC* m_node;
	bool m_recursive;
};