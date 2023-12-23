#include <States/TitleScreenState.h>
#include <States/LevelIntroState.h>
#include <GUI/LevelExit.h>

#include "LevelExitState.h"
#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

LevelExitState::LevelExitState(StateMachine& machine) : State(machine, States::LEVELEXIT) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_ui = Noesis::GUI::CreateView(LevelExit::Get().m_grid).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

LevelExitState::~LevelExitState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelExitState::fixedUpdate() {

}

void LevelExitState::update() {

}

void LevelExitState::render() {

	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	// Render
	m_ui->GetRenderer()->Render();
}

void LevelExitState::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void LevelExitState::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void LevelExitState::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelExitState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelExitState::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void LevelExitState::OnKeyDown(Event::KeyboardEvent& event) {

}

void LevelExitState::OnKeyUp(Event::KeyboardEvent& event) {

}

void LevelExitState::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtBottom(states == States::LEVELINTRO ? static_cast<State*>(new LevelIntroState(m_machine)) : static_cast<State*>(new TitleScreenState(m_machine)));
}