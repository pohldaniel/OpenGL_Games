#pragma once

#include "Event/input-handler.hpp"

class Game; // Forward declaration

enum class GameState {
	CINEMATIC,
	TITLE_SCREEN,
	LEVEL_INTRO,
	LEVEL,
	LEVEL_EXIT,
	GAME_OVER,
	END_SCREEN
};

class IGameState : public InputHandler {
public:
	IGameState(Game& game);

	virtual void enter() = 0;
	virtual void update(float deltatime) = 0;
	virtual void exit() = 0;

	void restoreGpuState();

protected:
	Game& m_game;
};
