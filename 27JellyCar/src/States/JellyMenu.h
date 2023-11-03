#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
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

	//void loadLevelInfo(std::string path);

	unsigned int backWidth = 0, backHeight = 0;
	unsigned int columns = 0, rows = 0;
	float controlsWidth = 0, controlsHeight = 0;


	int& currentPosition;
	int& carcurrentPosition;

	std::vector<SkinInfo> _carSkins;
	std::vector<std::string> _sceneFiles;

	LevelManager* _levelManager;
	std::vector<LevelSoftBody*> _gameBodies;
	World* _world;
	Car* _car;

	glm::mat4 _jellyProjection;

	unsigned int m_thumbAtlas = 0;

	

	const std::vector<std::string> thumbsFromLevelInfos(const std::vector<LevelInfo2>& levelInfos);
};