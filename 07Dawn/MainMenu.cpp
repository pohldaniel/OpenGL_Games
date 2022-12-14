#include "MainMenu.h"
#include "ChooseClassMenu.h"
#include "Utils.h"

MainMenu::MainMenu(StateMachine& machine) : State(machine, CurrentState::MAINMENU) {
	Mouse::SetCursorIcon("res/cursors/black.cur");

	m_dialog = Dialog(0, 0, 0, 0);
	m_dialog.setPosition(0, 0);

	m_dialog.setAutoresize();
	m_dialog.setCenteringLayout();
	m_dialog.setCenterOnScreen();

	m_dialog.addChildWidget(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Quit Game")));
	m_dialog.addChildWidget(0, 20, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Editor")));
	m_dialog.addChildWidget(0, 40, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Load Game")));
	m_dialog.addChildWidget(0, 60, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "New Game")));

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[1])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new LoadingScreen(m_machine, LoadingManager::Entry::EDITOR));
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setFunction([&]() {
		if (Utils::file_exists("res/_lua/save/savegame.lua")) {
			m_isRunning = false;
			m_machine.addStateAtTop(new LoadingScreen(m_machine, LoadingManager::Entry::LOAD));
		}
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[3])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new ChooseClassMenu(m_machine));
	});

	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("font")->getAtlas(), true, 0);
}

MainMenu::~MainMenu() {}

void MainMenu::fixedUpdate() {}

void MainMenu::update() {
	
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setDefaultColor(Utils::file_exists("res/_lua/save/savegame.lua") ? Vector4f(1.0f, 1.0f, 1.0f, 1.0f) : Vector4f(0.5f, 0.5f, 0.5f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setHoverColor(Utils::file_exists("res/_lua/save/savegame.lua") ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

	m_dialog.processInput();
}

void MainMenu::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_dialog.draw();
}

void MainMenu::resize(int deltaW, int deltaH) {
	m_dialog.applyLayout();
}