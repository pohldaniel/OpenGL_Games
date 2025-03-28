#pragma once

#include <random>
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Camera.h>
#include <engine/Sprite.h>
#include <engine/Framebuffer.h>

#include <States/StateMachine.h>
#include <Entities/Monster.h>
#include <UI/Icon.h>
#include <UI/Empty.h>

#include "MonsterIndex.h"
#include "Fade.h"

class Battle : public State, public MouseEventListener, public KeyboardEventListener, public ui::Icon {

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
	Fade& getFade();
	void setOpponentMonsters();
	void setOpponentMonsters(const std::vector<MonsterEntry>& monsters, bool canCatch = false);
	void setBiomeBackground(const std::string& biomeBackground);

private:

	void updateAbilityAnimation();
	void updateGeneral();
	void updateAttacks();
	void updateSwitch();
	void onAbilityEnd();
	void removeDefeteadMonster();
	void opponentAttack();
	void exit();
	void initUI();
	void addAbilities();
	void eraseAbilities();
	void addListMonsters();
	void eraseListMonsters();
	void addGeneralUI(float posX, float posY);
	void eraseGeneralUI();
	void addAttacksUI(float posX, float posY, float scaleX, float scaleY);
	void eraseAttacksUI();
	void addSwitchUI(float posX, float posY, float scaleX, float scaleY);
	void eraseSwitchUI();
	void addAbilityUI(float posX, float posY);
	void eraseAbilityUI();
	bool canSwitch();
	int getDefeated();
	bool growSupplyIndexPlayer();
	void playAttackSound(const std::string& attackName);

	void onDrawGeneralUI(bool flag);
	void onDrawAttacksUI(bool flag);
	void onDrawSwitchUI(bool flag);
	void onDrawAbilityUI(bool flag);

	bool m_playAbility;
	bool m_drawGeneralUi, m_drawAtacksUi, m_drawSwitchUi, m_drawTargetUI, m_removeDefeteadMonster, m_catchMonster, m_canSwitch, m_canCatch;
	bool m_exit;

	Camera m_camera;
	float m_viewWidth;
	float m_viewHeight;
	float m_mapHeight;
	float m_abilityPosX, m_abilityPosY;

	std::vector<Cell> m_cells;
	unsigned int m_atlasBattleIcon, m_atlasAbilities, m_backgrounds;
	int m_currentSelectedMonster;
	int m_currentSelectedOption, m_currentMax, m_currentOffset, m_cutOff;
	int m_visibleItems;
	std::pair<std::string, unsigned int> m_currentAbility;
	std::string m_currentTarget;
	tsl::ordered_map<std::string, unsigned int> m_abilitiesFiltered;
	std::vector<std::reference_wrapper<MonsterEntry>> m_filteredMonsters;

	std::vector<MonsterEntry> m_opponentMonsters;
	std::vector<Monster*> m_monsters;

	std::vector<std::array<float, 2>> positions;
	std::vector<std::array<float, 2>> centers;

	Sprite m_surface;
	Timer m_opponentTimer, m_exitTimer;

	float m_elapsedTime;
	int m_currentFrame, m_abilityOffset;
	int m_frameCount;

	unsigned int m_supplyIndexOpponent, m_supplyIndexPlayer;
	Fade m_fade;
	float m_alpha;
	Framebuffer m_mainRenderTarget;

	static std::random_device RandomDevice;
	static std::mt19937 MersenTwist;
	static Sprite SurfaceBar;
	static TextureRect BackgroundRect;
};