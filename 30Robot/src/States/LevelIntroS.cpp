#include "LevelIntroS.h"

#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

#include <Event/change-game-state.hpp>
#include <States/TitleScreen.h>
#include <States/Default.h>
#include <States/LevelS.h>

#include <GUI/LevelIntro.h>

LevelIntroS::LevelIntroS(StateMachine& machine) : State(machine, States::LEVELINTRO) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_xaml = LevelIntro::Get().m_grid;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

LevelIntroS::~LevelIntroS() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelIntroS::fixedUpdate() {

}

void LevelIntroS::update() {

}

void LevelIntroS::render() {

	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	// Render
	m_ui->GetRenderer()->Render();
}

void LevelIntroS::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void LevelIntroS::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void LevelIntroS::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelIntroS::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelIntroS::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void LevelIntroS::OnKeyDown(Event::KeyboardEvent& event) {

}

void LevelIntroS::OnKeyUp(Event::KeyboardEvent& event) {

}

void LevelIntroS::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtBottom(states == States::LEVEL ? static_cast<State*>(new LevelS(m_machine)) : static_cast<State*>(new TitleScreenS(m_machine)));

	/*if (states == States::LEVEL) {
		m_machine.addStateAtTop(new LevelS(m_machine));
		return;
	}

	if (states == States::TITLESCREEN) {
		m_isRunning = false;
		m_machine.addStateAtBottom(new TitleScreenS(m_machine));
	}*/
}