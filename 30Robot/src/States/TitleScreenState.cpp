#include <States/TitleScreenState.h>
#include <States/LevelIntroState.h>
#include <GUI/TitleScreen.h>
#include <Level/Level.h>

#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

TitleScreenConnections::TitleScreenConnections() {
	Application::Emitter.on<evnt::ChangeGameState>([this](const evnt::ChangeGameState& event, EventEmitter& emitter) {
		if(event.levelNumber != 0) Application::s_Level->setLevel(event.levelNumber);
		dynamic_cast<EventListener*>(Application::GetMachine()->getStates().top())->OnStateChange(event.state);
	});

	Application::Emitter.on<evnt::ApplicationExit>([this](const evnt::ApplicationExit& event, EventEmitter& emitter) {
		dynamic_cast<EventListener*>(Application::GetMachine()->getStates().top())->OnApplicationQuit();
	});
}

TitleScreenConnections TitleScreenState::TitleScreenSConnections;

TitleScreenState::TitleScreenState(StateMachine& machine) : State(machine, States::TITLESCREEN) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_ui = Noesis::GUI::CreateView(TitleScreen::Get().m_grid).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

TitleScreenState::~TitleScreenState() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	TitleScreen::Get().clearAnimations();

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void TitleScreenState::fixedUpdate() {

}

void TitleScreenState::update() {

}

void TitleScreenState::render() {

	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	// Render
	m_ui->GetRenderer()->Render();
}

void TitleScreenState::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void TitleScreenState::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void TitleScreenState::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void TitleScreenState::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void TitleScreenState::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void TitleScreenState::OnKeyDown(Event::KeyboardEvent& event) {

}

void TitleScreenState::OnKeyUp(Event::KeyboardEvent& event) {

}

void TitleScreenState::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtBottom(new LevelIntroState(m_machine));
}

void TitleScreenState::OnApplicationQuit() {
	m_isRunning = false;
}