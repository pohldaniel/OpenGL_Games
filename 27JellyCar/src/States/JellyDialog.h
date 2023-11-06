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

class JellyDialog : public State {

public:

	JellyDialog(StateMachine& machine, Framebuffer& mainRT, std::string text);
	~JellyDialog();

	void fixedUpdate() override;
	void update() override;
	void render() override;
	void resize(int deltaW, int deltaH) override;

private:

	float controlsWidth = 0, controlsHeight = 0;
	std::string m_text;

	Framebuffer& mainRT;
};