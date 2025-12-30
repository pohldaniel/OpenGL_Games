#pragma once
#include <string>
#include <engine/Vector.h>
#include <engine/Rect.h>

#include "DataStructs.h"
#include "Entity_new.h"

enum ViewDirection {
	LEFT,
	RIGHT
};

class SpriteEntity : public EntityNew {

public:

	SpriteEntity(Cell& cell, float elpasedTime = 0.0f, int framecount = 4);
	SpriteEntity& operator=(const SpriteEntity& rhs);
	SpriteEntity& operator=(SpriteEntity&& rhs) noexcept;
	SpriteEntity(SpriteEntity const& rhs);
	SpriteEntity(SpriteEntity&& rhs) noexcept;

	virtual ~SpriteEntity();
	
	const Cell& getCell() const;

	static bool HasCollision(float r1_l, float r1_t, float r1_r, float r1_b, float r2_l, float r2_t, float r2_r, float r2_b);

protected:

	virtual Rect getRect();

	void updateAnimation(float dt);
	void resetAnimation();

	Cell& cell;
	int m_startFrame;
	int m_frameCount;
	float m_elapsedTime;

	ViewDirection m_viewDirection;
	Rect m_previousRect;
};