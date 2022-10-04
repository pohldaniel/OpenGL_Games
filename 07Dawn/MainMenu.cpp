#include "MainMenu.h"

MainMenu::MainMenu(StateMachine& machine) : State(machine, CurrentState::MAINMENU) {
	m_dialog = Dialog(0, 0, 0, 0);
	m_dialog.setPosition(200, 322);

	m_dialog.setAutoresize();
	m_dialog.setCenteringLayout();
	m_dialog.setCenterOnScreen();

	m_dialog.addChildFrame(0, 0, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Quit Game")));
	m_dialog.addChildFrame(0, 20, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Options")));
	m_dialog.addChildFrame(0, 40, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Editor")));
	m_dialog.addChildFrame(0, 60, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "New Game")));

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[2])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new LoadingScreen(m_machine));
	});

	/*dynamic_cast<Label*>(m_dialog.getChildWidgets()[3])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new Game(m_machine));
	});*/
}

MainMenu::~MainMenu() {}

void MainMenu::fixedUpdate() {}

void MainMenu::update() {
	ViewPort::get().update(m_dt);
	m_dialog.update(static_cast<int>(ViewPort::get().getCursorPos()[0]), static_cast<int>(ViewPort::get().getCursorPos()[1]));
}

void MainMenu::render(unsigned int &frameBuffer) {

	glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_BLEND);

	m_dialog.draw();

	glDisable(GL_BLEND);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void MainMenu::resize() {
	m_dialog.applyLayout();
}