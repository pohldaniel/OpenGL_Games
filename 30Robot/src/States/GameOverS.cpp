#include "GameOverS.h"

#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

#include <Event/change-game-state.hpp>
#include <States/TitleScreen.h>
#include <States/LevelIntroS.h>

GameOverS::GameOverS(StateMachine& machine) : State(machine, States::GAMEOVER), m_gameOver(Application::Emitter, Application::s_Progression) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_xaml = m_gameOver;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

GameOverS::~GameOverS() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void GameOverS::fixedUpdate() {

}

void GameOverS::update() {

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