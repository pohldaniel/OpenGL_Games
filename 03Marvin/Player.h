#pragma once
#include <Box2D\Box2D.h>
#include "Constants.h"

class Player {
public:
	Player();
	~Player();

	void render();

	b2Body* m_playerBody;
};