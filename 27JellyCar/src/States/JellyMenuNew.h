#pragma once
#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/input/Mouse.h>

#include <States/StateMachine.h>
#include <UI/Button.h>
#include <UI/TextField.h>


#include <glm/gtc/matrix_transform.hpp>

#include "JellyHelper.h"
#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

class JellyMenuNew : public State, public MouseEventListener, public KeyboardEventListener {

public:
	JellyMenuNew(StateMachine& machine);
	~JellyMenuNew();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

	void processInput();

private:

	void OnMouseMotion(Event::MouseMoveEvent& event) override;
	void OnMouseButtonDown(Event::MouseButtonEvent& event) override;
	void OnKeyDown(Event::KeyboardEvent& event) override;

	unsigned int backWidth = 0, backHeight = 0;
	unsigned int columns = 0, rows = 0;

	int currentPosition;
	int columnStartPosition;
	int positionsInColumn;

	int carcurrentPosition;
	int carcolumnStartPosition;
	int carpositionsInColumn;
	std::vector<SkinInfo> _carSkins;
	std::vector<std::string> _sceneFiles;

	LevelManager* _levelManager;
	std::vector<LevelSoftBody*> _gameBodies;
	World* _world;
	Car* _car;

	glm::mat4 _jellyProjection;

	unsigned int m_thumbAtlas = 0;
};