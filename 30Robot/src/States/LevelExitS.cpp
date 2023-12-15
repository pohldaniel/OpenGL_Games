#include <NsGui/IntegrationAPI.h>
#include <States/TitleScreenS.h>
#include <States/Default.h>
#include <States/LevelS.h>
#include <GUI/LevelExit.h>

#include "LevelExitS.h"
#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

LevelExitS::LevelExitS(StateMachine& machine) : State(machine, States::LEVELEXIT) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_xaml = LevelExit::Get().m_grid;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

LevelExitS::~LevelExitS() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
}

void LevelExitS::fixedUpdate() {

}

void LevelExitS::update() {

}

void LevelExitS::render() {

	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	// Render
	m_ui->GetRenderer()->Render();
}

void LevelExitS::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void LevelExitS::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void LevelExitS::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelExitS::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void LevelExitS::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void LevelExitS::OnKeyDown(Event::KeyboardEvent& event) {

}

void LevelExitS::OnKeyUp(Event::KeyboardEvent& event) {

}

void LevelExitS::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtBottom(states == States::LEVEL ? static_cast<State*>(new LevelS(m_machine)) : static_cast<State*>(new TitleScreenS(m_machine)));

	/*if (states == States::LEVEL) {
		m_isRunning = false;
		m_machine.addStateAtTop(new LevelS(m_machine));
		return;
	}

	if (states == States::TITLESCREEN) {
		m_machine.clearAndPush(new TitleScreenS(m_machine));
	}*/
}