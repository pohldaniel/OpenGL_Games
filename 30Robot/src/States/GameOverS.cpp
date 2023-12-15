#include <NsGui/IntegrationAPI.h>
#include <States/TitleScreenS.h>
#include <States/LevelIntroS.h>
#include <GUI/GameOver.h>

#include "GameOverS.h"
#include "Application.h"
#include "Globals.h"
#include "Renderer.h"



GameOverS::GameOverS(StateMachine& machine) : State(machine, States::GAMEOVER) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_xaml = GameOver::Get().m_grid;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

GameOverS::~GameOverS() {
	std::cout << "Destructor Game Over: " << std::endl;

	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void GameOverS::fixedUpdate() {

}

void GameOverS::update() {
	Keyboard &keyboard = Keyboard::instance();

	if (keyboard.keyPressed(Keyboard::KEY_W)) {
		m_isRunning = false;
		//m_machine.addStateAtBottom(states == States::LEVELINTRO ? static_cast<State*>(new LevelIntroS(m_machine)) : static_cast<State*>(new TitleScreenS(m_machine)));
		m_machine.addStateAtBottom(static_cast<State*>(new LevelIntroS(m_machine)));
	}
}

void GameOverS::render() {

	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	// Render
	m_ui->GetRenderer()->Render();
}

void GameOverS::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void GameOverS::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void GameOverS::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void GameOverS::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void GameOverS::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void GameOverS::OnKeyDown(Event::KeyboardEvent& event) {

}

void GameOverS::OnKeyUp(Event::KeyboardEvent& event) {

}

void GameOverS::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtBottom(states == States::LEVELINTRO ? static_cast<State*>(new LevelIntroS(m_machine)) : static_cast<State*>(new TitleScreenS(m_machine)));

	/*if (states == States::LEVELINTRO) {
		m_isRunning = false;
		return;
	}

	if (states == States::TITLESCREEN) {
		m_machine.clearAndPush(new TitleScreenS(m_machine));
	}*/	
}