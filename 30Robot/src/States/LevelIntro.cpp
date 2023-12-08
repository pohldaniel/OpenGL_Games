#include "LevelIntro.h"

#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

LevelIntroS::LevelIntroS(StateMachine& machine) : State(machine, States::TITLESCREEN), InputHandler(Application::Emitter), m_levelIntro(Application::Emitter, Application::s_Progression) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_xaml = m_levelIntro;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

LevelIntroS::~LevelIntroS() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
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

void LevelIntroS::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void LevelIntroS::enter() {
	connectInputs();
}

void LevelIntroS::exit() {
	disconnectInputs();
}