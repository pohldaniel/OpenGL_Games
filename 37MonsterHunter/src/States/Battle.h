#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Sprite.h>

#include <States/StateMachine.h>
#include <Entities/Monster.h>
#include "MonsterIndex.h"

struct BattleChoice {
	Vector2f pos;
	unsigned int graphics;
};

class Battle : public State, public MouseEventListener, public KeyboardEventListener {

	static const int MAX_POINTS = 1000;

public:

	Battle(StateMachine& machine);
	~Battle();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void processInput();

	void resize(int deltaW, int deltaH) override;
	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseWheel(Event::MouseWheelEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnMouseButtonUp(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;
	void OnKeyUp(Event::KeyboardEvent& event) override;

	void setMapHeight(float mapHeight);
	void setViewHeight(float viewHeight);

private:

	void drawAbilityAnimation(float posX, float posY);
	void renderUi();
	void drawGeneral();
	void drawAttacks();
	void drawSwitch();
	void onAbilityEnd();
	void removeDefeteadMonster();

	bool m_initUi = true;
	bool m_drawUi = false;
	bool m_playAbility;
	bool m_drawGeneralUi, m_drawAtacksUi, m_drawSwitchUi, m_drawTargetUI;

	Camera m_camera;
	float m_viewWidth;
	float m_viewHeight;
	float m_mapHeight;
	float m_abilityPosX, m_abilityPosY;

	std::vector<Cell> m_cells;
	unsigned int m_atlasBattleIcon, m_atlasAbilities;
	int m_currentSelectedMonster;
	int m_currentSelectedOption, m_currentMax, m_currentOffset, m_cutOff;
	int m_visibleItems;
	std::pair<std::string, unsigned int> m_currentAbility;
	std::string m_currentTarget;

	tsl::ordered_map<std::string, unsigned int> m_abilitiesFiltered;
	std::vector<MonsterEntry> m_filteredMonsters;

	std::vector<MonsterEntry> m_opponentMonster;
	std::vector<Monster> m_monster;
	std::vector<std::array<float, 2>> positions;
	std::vector<std::array<float, 2>> centers;
	std::vector<BattleChoice> m_battleChoices;

	Sprite m_surface;

	float m_elapsedTime;
	int m_currentFrame, m_abilityOffset;
	int m_frameCount;
};