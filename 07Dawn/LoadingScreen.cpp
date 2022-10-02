#include "LoadingScreen.h"

LoadingScreen::LoadingScreen(StateMachine& machine, LoadingManager* loadingManager) : State(machine, CurrentState::LOADINGSCREEN) {
	this->loadingManager = loadingManager;

	loadingManager->startBackgroundThread();
}

LoadingScreen::~LoadingScreen() {}

void LoadingScreen::fixedUpdate() {

}

void LoadingScreen::update() {
	if (!loadingManager->isFinished()){
		std::cout << loadingManager->getActivityText() << std::endl;

	}/*else{
		setDone();
	}*/
}

void LoadingScreen::render(unsigned int &frameBuffer) {
	
}