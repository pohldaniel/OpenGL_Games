#pragma once

#include "../engine/scene/SceneNodeLC.h"

class Navigable {

	//URHO3D_OBJECT(Navigable, Component);

public:
	/// Construct.
	Navigable(SceneNodeLC* node);
	/// Destruct.
	virtual ~Navigable();
	/// Register object factory.
	//static void RegisterObject();

	/// Set whether geometry is automatically collected from child nodes. Default true.
	void SetRecursive(bool enable);

	/// Return whether geometry is automatically collected from child nodes.
	bool IsRecursive() const { return recursive_; }

	SceneNodeLC* m_node;

private:
	/// Recursive flag.
	bool recursive_;
};