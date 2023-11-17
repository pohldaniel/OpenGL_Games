#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Vector.h>
#include <States/StateMachine.h>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "JellyPhysics/World.h"
#include "SceneManager.h"


class JellyMenu : public State, public MouseEventListener, public KeyboardEventListener {

public:

	JellyMenu(StateMachine& machine);
	~JellyMenu();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;
	
private:

	void OnKeyDown(Event::KeyboardEvent& event) override;
	void processInput();

	unsigned int m_backWidth, m_backHeight;
	unsigned int m_columns, m_rows;
	float m_controlsWidth, m_controlsHeight;


	int& currentPosition;
	int& carcurrentPosition;

	unsigned int m_levelInfosSize;
	unsigned int m_carSkinsSize;

	std::vector<LevelSoftBody*> m_gameBodies;
	World* m_world;
	Car* m_car;
	Matrix4f m_jellyProjection;
	unsigned int m_thumbAtlas = 0;
};