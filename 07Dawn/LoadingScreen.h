#pragma once
#include "StateMachine.h"
#include "Loadingmanager.h"

class LoadingScreen : public State {
public:
	LoadingScreen(StateMachine& machine, LoadingManager* loadingManager);
	~LoadingScreen();

	virtual void fixedUpdate() override;
	virtual void update() override;
	virtual void render(unsigned int &frameBuffer) override;

private:
	LoadingManager* loadingManager;
};