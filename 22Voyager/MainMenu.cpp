#include "MainMenu.h"
#include "Application.h"

MainMenu::MainMenu(StateMachine& machine) : State(machine, CurrentState::MAINMENU) {
	Application::SetCursorIcon("res/cursors/black.cur");
	EventDispatcher::AddMouseListener(this);
}

MainMenu::~MainMenu() {
	EventDispatcher::RemoveMouseListener(this);
}

void MainMenu::fixedUpdate() {}

void MainMenu::update() {
	processInput();

}

void MainMenu::render() {

}

void MainMenu::processInput() {
	Keyboard &keyboard = Keyboard::instance();	

	if (keyboard.keyPressed(Keyboard::KEY_F)) {

	}
}

void MainMenu::resize(int deltaW, int deltaH) {

}

void MainMenu::OnMouseMotion(Event::MouseMoveEvent& event) {

}

void MainMenu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	
}