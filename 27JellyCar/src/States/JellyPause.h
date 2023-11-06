#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <engine/Framebuffer.h>
#include <States/StateMachine.h>

#include <glm/gtc/matrix_transform.hpp>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

class JellyPause : public State {

public:

	JellyPause(StateMachine& machine, const Framebuffer& mainRT);
	~JellyPause();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	const Framebuffer& mainRT;
	float controlsWidth = 0, controlsHeight = 0;
	bool _checkpoint;
};