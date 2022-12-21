#include "ChooseClassMenu.h"
#include "MainMenu.h"

ChooseClassMenu::ChooseClassMenu(StateMachine& machine) : State(machine, CurrentState::CHOOSECLASSMENU) {
	m_dialog = Dialog(0, 0, 0, 0);
	m_dialog.setPosition(0, 0);

	m_dialog.setAutoresize();
	m_dialog.setCenteringLayout();
	m_dialog.setCenterOnScreen();

	m_dialog.addChildWidget(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Back")));
	m_dialog.addChildWidget(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Warrior")));
	m_dialog.addChildWidget(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Ranger")));
	m_dialog.addChildWidget(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Liche")));
	m_dialog.addChildWidget(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Choose class")));

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[4])->setDefaultColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[4])->setHoverColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[0])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new MainMenu(m_machine));
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[1])->setFunction([&]() {
		Player::Get().setCharacterType("player_s");
		Player::Get().setClass(Enums::CharacterClass::Warrior);
		m_isRunning = false;
		m_machine.addStateAtTop(new LoadingScreen(m_machine));
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setFunction([&]() {
		Player::Get().setCharacterType("player_r");
		Player::Get().setClass(Enums::CharacterClass::Ranger);
		m_isRunning = false;
		m_machine.addStateAtTop(new LoadingScreen(m_machine));
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[3])->setFunction([&]() {
		Player::Get().setCharacterType("player_w");
		Player::Get().setClass(Enums::CharacterClass::Liche);
		m_isRunning = false;
		m_machine.addStateAtTop(new LoadingScreen(m_machine));
	});

	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("font")->getAtlas(), true, 0);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("dialog"), true, 2);
}

ChooseClassMenu::~ChooseClassMenu() {}

void ChooseClassMenu::fixedUpdate() {}

void ChooseClassMenu::update() {
	m_dialog.processInput();
}

void ChooseClassMenu::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_dialog.draw();
	TextureManager::DrawBuffer();
}

void ChooseClassMenu::resize(int deltaW, int deltaH) {
	m_dialog.applyLayout();
}