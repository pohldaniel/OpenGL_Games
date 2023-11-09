#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

class JellyIntro : public State {

public:

	JellyIntro(StateMachine& machine);
	~JellyIntro();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	unsigned int m_backWidth, m_backHeight;
	unsigned int m_columns, m_rows;

	std::vector<SkinInfo> m_carSkins;
	LevelManager* m_levelManager;
	std::vector<LevelSoftBody*> m_gameBodies;
	World* m_world;
	Car* m_car;

	glm::mat4 m_jellyProjection;

	Vector2 m_levelTarget;
};