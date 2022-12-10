#include "MainMenu.h"
#include "Interface.h"
#include "Spellbook.h"
#include "Player.h"

MainMenu::MainMenu(StateMachine& machine) : State(machine, CurrentState::MAINMENU) {
	Mouse::SetCursorIcon("res/cursors/black.cur");

	m_dialog = Dialog(0, 0, 0, 0);
	m_dialog.setPosition(0, 0);

	m_dialog.setAutoresize();
	m_dialog.setCenteringLayout();
	m_dialog.setCenterOnScreen();

	m_dialog.addChildWidget(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Quit Game")));
	m_dialog.addChildWidget(0, 20, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Options")));
	m_dialog.addChildWidget(0, 40, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Editor")));
	m_dialog.addChildWidget(0, 60, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "New Game")));

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new Editor(m_machine));
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[3])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new LoadingScreen(m_machine));
	});

	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("font")->getAtlas(), true, 0);
}

MainMenu::~MainMenu() {}

void MainMenu::fixedUpdate() {}

void MainMenu::update() {
	m_dialog.processInput();
}

void MainMenu::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	m_dialog.draw();
}

void MainMenu::resize(int deltaW, int deltaH) {
	m_dialog.applyLayout();
}