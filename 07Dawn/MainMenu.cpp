#include "MainMenu.h"
#include "Interface.h"
#include "Spellbook.h"
#include "Player.h"

MainMenu::MainMenu(StateMachine& machine) : State(machine, CurrentState::MAINMENU) {
	Mouse::SetCursorIcon("res/cursors/black.cur");

	LuaFunctions::executeLuaFile("res/_lua/playerdata_w.lua");
	Player::Get().setCharacterType("player_w");
	Player::Get().setClass(Enums::CharacterClass::Liche);


	Interface::Get().setPlayer(&Player::Get());
	Spellbook::Get().setPlayer(&Player::Get());

	LuaFunctions::executeLuaFile("res/_lua/spells_l.lua");
	LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");
	

	ZoneManager::Get().getZone("res/_lua/zone1").loadZone();
	ZoneManager::Get().setCurrentZone(&ZoneManager::Get().getZone("res/_lua/zone1"));
	
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
		m_machine.addStateAtTop(new LoadingScreen(m_machine));
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[3])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new Game(m_machine));
	});
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