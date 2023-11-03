#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

class JellyGame : public State {

public:

	JellyGame(StateMachine& machine);
	~JellyGame();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	unsigned int backWidth = 0, backHeight = 0;
	unsigned int columns = 0, rows = 0;

	std::vector<SkinInfo> _carSkins;
	LevelManager* _levelManager;
	std::vector<LevelSoftBody*> _gameBodies;
	World* _world;
	Car* _car;

	glm::mat4 _jellyProjection;

	Vector2 _levelTarget;
	bool _end;

};