#pragma once

#include "../Object/ObjectTu.h"

class AnimationController : public ObjectTu
{
	OBJECT(DebugRenderer);

public:
	/// Construct. Register subsystem. Graphics subsystem must have been initialized.
	AnimationController();
	/// Destruct.
	~AnimationController();
};