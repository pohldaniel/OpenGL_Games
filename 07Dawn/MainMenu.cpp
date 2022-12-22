#include "MainMenu.h"
#include "ChooseClassMenu.h"
#include "Options.h"
#include "Utils.h"

MainMenu::MainMenu(StateMachine& machine) : State(machine, CurrentState::MAINMENU) {
	EventDispatcher::AddMouseListener(this);

	m_dialog = Dialog(0, 0, 0, 0);
	m_dialog.setPosition(0, 0);

	m_dialog.setAutoresize();
	m_dialog.setCenteringLayout();
	m_dialog.setCenterOnScreen();

	m_dialog.addChildWidget(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Quit Game")));
	m_dialog.addChildWidget(0, 20, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Editor")));
	m_dialog.addChildWidget(0, 40, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Options")));
	m_dialog.addChildWidget(0, 60, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Load Game")));
	m_dialog.addChildWidget(0, 80, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "New Game")));

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[1])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new LoadingScreen(m_machine, LoadingManager::Entry::EDITOR));
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new Options(m_machine));
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setDefaultColor(Utils::file_exists("res/_lua/save/savegame.lua") ? Vector4f(1.0f, 1.0f, 1.0f, 1.0f) : Vector4f(0.5f, 0.5f, 0.5f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setHoverColor(Utils::file_exists("res/_lua/save/savegame.lua") ? Vector4f(1.0f, 1.0f, 0.0f, 1.0f) : Vector4f(1.0f, 0.0f, 0.0f, 1.0f));

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[3])->setFunction([&]() {
		if (Utils::file_exists("res/_lua/save/savegame.lua")) {
			m_isRunning = false;
			m_machine.addStateAtBottom(new LoadingScreen(m_machine, LoadingManager::Entry::LOAD));
		}
	});

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[4])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new ChooseClassMenu(m_machine));
	});

	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("font")->getAtlas(), true, 0);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("dialog"), true, 2);
	//Globals::musicManager.get("background").play("res/music/Early_Dawn_Simple.ogg");
}

MainMenu::~MainMenu() {
	EventDispatcher::RemoveMouseListener(this);
}

void MainMenu::fixedUpdate() {}

void MainMenu::update() {}

void MainMenu::render() {

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_dialog.draw();
	TextureManager::DrawBuffer();
}

void MainMenu::processInput() {
	Keyboard &keyboard = Keyboard::instance();
	
	if (keyboard.keyPressed(Keyboard::KEY_1)) {
		//Globals::musicManager.get("background").play("res/music/loading.ogg");
		//Globals::soundManager.get("player").play("res/sound/arrowHit06.ogg");
		Globals::soundEffectsPlayer.get("effect_2").play();
	}
	
	if (keyboard.keyPressed(Keyboard::KEY_2)) {
		Globals::musicManager.get("background").play("res/music/Early_Dawn_Simple.ogg");
		//Globals::soundManager.get("player").play("res/sound/click.ogg");
	}

	if (keyboard.keyPressed(Keyboard::KEY_3)) {
		Globals::musicManager.get("foreground").play("res/music/mika_FallToPieces_Silence.ogg");
		//Globals::soundManager.get("player").play("res/sound/quiet_click.ogg");
	}

	if (keyboard.keyPressed(Keyboard::KEY_4)) {
		Globals::musicManager.get("foreground").play("res/music/ratsrats_0.ogg");
		//Globals::soundManager.get("player").playChanneled("res/sound/sell_buy_item.ogg");
	}
	
	if (keyboard.keyPressed(Keyboard::KEY_5)) {
		Globals::soundManager.get("effect").playParallel("res/sound/spellcast.ogg");
	}

	if (keyboard.keyPressed(Keyboard::KEY_6)) {
		Globals::soundManager.get("effect").playParallel("res/sound/sword_01.ogg");
	}

	if (keyboard.keyPressed(Keyboard::KEY_7)) {
		Globals::soundManager.get("effect").playParallel("res/sound/walking.ogg");
	}
}

void MainMenu::resize(int deltaW, int deltaH) {
	m_dialog.applyLayout();
}

void MainMenu::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_dialog.processInput(event.x, ViewPort::Get().getHeight() - event.y);
}

void MainMenu::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_dialog.processInput(event.x, ViewPort::Get().getHeight() - event.y, event.button);
}