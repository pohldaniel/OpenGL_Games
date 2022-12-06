#pragma once

#include "engine/Batchrenderer.h"
#include "StateMachine.h"

class Game : public State {

public:

	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render(unsigned int &frameBuffer) override;
	void resize(int deltaW, int deltaH) override;

private:

	void processInput();
};