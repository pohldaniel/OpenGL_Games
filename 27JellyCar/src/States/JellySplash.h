#pragma once

#include <engine/input/MouseEventListener.h>
#include <engine/input/KeyboardEventListener.h>
#include <States/StateMachine.h>

#include "Car/Car.h"
#include "Levels/LevelSoftBody.h"
#include "Levels/LevelManager.h"
#include "JellyPhysics/World.h"

class JellySplash : public State {

public:
	JellySplash(StateMachine& machine);
	~JellySplash();

	void fixedUpdate() override;
	void update() override;
	void render() override;

private:
	
	bool _end;
	float _dt;
	float _alpha;
	float _splashTimer;
};