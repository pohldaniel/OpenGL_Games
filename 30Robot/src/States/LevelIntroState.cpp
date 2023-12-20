#include <States/TitleScreenState.h>
#include <States/LevelState.h>
#include <States/LevelIntroState.h>
#include <GUI/LevelIntro.h>

#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

LevelIntroState::LevelIntroState(StateMachine& machine) : State(machine, States::LEVELINTRO) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_ui = Noesis::GUI::CreateView(LevelIntro::Get().m_grid).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

LevelIntroState::~LevelIntroState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelIntroState::fixedUpdate() {

}

void LevelIntroState::update() {

}

void LevelIntroState::render() {

	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	// Render
	m_ui->GetRenderer()->Render();
}

void LevelIntroState::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void LevelIntroState::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void LevelIntroState::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelIntroState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelIntroState::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void LevelIntroState::OnKeyDown(Event::KeyboardEvent& event) {

}

void LevelIntroState::OnKeyUp(Event::KeyboardEvent& event) {

}

void LevelIntroState::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtBottom(states == States::LEVEL ? static_cast<State*>(new LevelState(m_machine)) : static_cast<State*>(new TitleScreenState(m_machine)));
}