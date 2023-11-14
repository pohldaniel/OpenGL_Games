#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"
#include "Loadingmanager.h"


class JellyIntro : public State {

	struct LoadSceneAndThumbsTask : public MemberFunctionTask<JellyIntro> {
		LoadSceneAndThumbsTask(JellyIntro* object_, MemberWorkFunctionPtr OnProcess_, MemberWorkFunctionPtr OnComplete_) : MemberFunctionTask<JellyIntro>(object_, OnProcess_, OnComplete_) { }
	};

public:

	JellyIntro(StateMachine& machine);
	~JellyIntro();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

	void OnComplete();
	void OnProcess();

private:

	unsigned int m_backWidth, m_backHeight;
	unsigned int m_columns, m_rows;

	LevelManager* m_copyManager;

	std::vector<LevelSoftBody*> m_gameBodies;
	World* m_world;
	World* m_copyWorld;
	Car* m_car;

	glm::mat4 m_jellyProjection;

	Vector2 m_levelTarget;
};