#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/input/EventDispatcher.h"
#include "engine/Batchrenderer.h"

#include "StateMachine.h"
#include "Interface.h"
#include "Player.h"
#include "Spells.h"
#include "Zone.h"

class Game : public State, public MouseEventListener {
public:
	Game(StateMachine& machine);
	~Game();

	void fixedUpdate() override;
	void update() override;
	void render(unsigned int &frameBuffer) override;
	void resize(int deltaW, int deltaH) override;

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	

	//Zone* zone;
	//Interface* dawnInterface;
	Player& m_player;

	bool m_drawInGameCursor = false;

private:
	void processInput();
};