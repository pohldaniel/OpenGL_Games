#include <NsGui/IntegrationAPI.h>
#include <States/TitleScreenState.h>
#include <States/LevelIntroState.h>
#include <GUI/GameOver.h>

#include "GameOverState.h"
#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

GameOverState::GameOverState(StateMachine& machine) : State(machine, States::GAMEOVER) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_ui = Noesis::GUI::CreateView(GameOver::Get().m_grid).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

GameOverState::~GameOverState() {
	std::cout << "Destructor Game Over: " << std::endl;

	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void GameOverState::fixedUpdate() {

}

void GameOverState::update() {
	Keyboard &keyboard = Keyboard::instance();
}

void GameOverState::render() {

	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	// Render
	m_ui->GetRenderer()->Render();
}

void GameOverState::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void GameOverState::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void GameOverState::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void GameOverState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void GameOverState::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void GameOverState::OnKeyDown(Event::KeyboardEvent& event) {

}

void GameOverState::OnKeyUp(Event::KeyboardEvent& event) {

}

void GameOverState::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtBottom(states == States::LEVELINTRO ? static_cast<State*>(new LevelIntroState(m_machine)) : static_cast<State*>(new TitleScreenState(m_machine)));
}