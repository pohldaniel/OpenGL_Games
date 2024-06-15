#pragma once

class SpriteEntity {

public:

	SpriteEntity();
	virtual ~SpriteEntity();

	virtual void update(float dt);
};