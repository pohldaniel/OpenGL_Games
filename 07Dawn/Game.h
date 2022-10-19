#pragma once

#include "engine/input/MouseEventListener.h"
#include "engine/input/KeyBorad.h"
#include "engine/input/Mouse.h"
#include "engine/Batchrenderer.h"
#include "engine/Transform.h"

#include "StateMachine.h"
#include "Interface.h"
#include "Player.h"
#include "Spells.h"

class Game : public State, public MouseEventListener {
public:
	Game(StateMachine& machine);
	~Game();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;

	Zone* zone;
	Interface* dawnInterface;
	Player& player;

	bool m_drawInGameCursor = false;

	GeneralRayDamageSpell* spell;
	Transform transform;
};