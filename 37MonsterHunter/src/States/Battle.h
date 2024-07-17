#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Background.h>

#include <States/StateMachine.h>
#include <Entities/Monster.h>
#include "MonsterIndex.h"

class Battle : public State, public MouseEventListener, public KeyboardEventListener {

public:

	Battle(StateMachine& machine);
	~Battle();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

	void setMapHeight(float mapHeight);

private:

	void renderUi();

	bool m_initUi = true;
	bool m_drawUi = false;

	Camera m_camera;
	float m_viewWidth;
	float m_viewHeight;
	float m_mapHeight;

	std::vector<Cell> m_cells;

	std::vector<MonsterEntry> m_playerMonster;
	std::vector<MonsterEntry> m_opponentMonster;
	std::vector<Monster> m_monster;
};