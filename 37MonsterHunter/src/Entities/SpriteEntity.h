#pragma once

#include "../Zone.h"

class SpriteEntity {

public:

	SpriteEntity(Cell& cell, float elpasedTime = 0.0f, int framecount = 4);
	virtual ~SpriteEntity();

	virtual void update(float dt) = 0;

	const Cell& getCell();

protected:

	Cell& cell;
	int m_startFrame;
	float m_elapsedTime;
	int m_frameCount;
};