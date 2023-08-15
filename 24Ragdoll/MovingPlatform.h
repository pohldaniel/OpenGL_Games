#pragma once

#include "turso3d/Scene/SpatialNode.h"
#include "turso3d/Renderer/StaticModel.h"

class MovingPlatform  {
	
public:

	MovingPlatform();
	~MovingPlatform();
	void FixedUpdate(float timeStep);

	void Initialize(StaticModel* liftNode, const Vector3 &finishPosition, bool updateBodyOnPlatform = true);
	void SetPlatformSpeed(float speed) { maxLiftSpeed_ = speed; }

protected:

	WeakPtr<StaticModel> platformNode_;

	Vector3 initialPosition_;
	Vector3 finishPosition_;
	Vector3 directionToFinish_;
	float maxLiftSpeed_;
	float minLiftSpeed_;
	float curLiftSpeed_;

	int platformState_;
	enum PlatformStateType
	{
		PLATFORM_STATE_START,
		PLATFORM_STATE_MOVETO_FINISH,
		PLATFORM_STATE_MOVETO_START,
		PLATFORM_STATE_FINISH
	};
};