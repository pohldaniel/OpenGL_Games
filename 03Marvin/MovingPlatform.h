#pragma once
#include <Box2D\Box2D.h>
#include "Constants.h"

class MovingPlatform {
public:

	MovingPlatform(const float& dt, const float& fdt);
	void fixedUpdate();
	void render();

	b2Body *platformBody;
	b2Vec2 platformPosition = b2Vec2(800.0f, 500.0f);

	b2Vec2 initialPosition_ = b2Vec2(700.0f, 500.0f);
	b2Vec2 finishPosition_ = b2Vec2(700.0f, 700.0f);
	b2Vec2 directionToFinish_;
	float maxLiftSpeed_ = 1.0f;
	float minLiftSpeed_ = 0.1f;
	float curLiftSpeed_ = 1.0f;
	float m_speed = 100.0f;
	const float& m_fdt;
	const float& m_dt;

	int platformState_ = PLATFORM_STATE_MOVETO_FINISH;
	enum PlatformStateType
	{
		PLATFORM_STATE_START,
		PLATFORM_STATE_MOVETO_FINISH,
		PLATFORM_STATE_MOVETO_START,
		PLATFORM_STATE_FINISH
	};

	inline float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		else if (value > max)
			return max;
		else
			return value;
	}
};
