#pragma once

#include "Constants.h"
#include "StateMachine.h"
#include "ContactFilter.h"
#include "CollisionHandler.h"
#include "Level.h"
#include "Player.h"

class Game : public State {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &m_frameBuffer) override;

	Level* m_level;
	Player* m_player;
};