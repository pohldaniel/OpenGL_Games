#include "TitleScreen.h"

#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

TitleScreenS::TitleScreenS(StateMachine& machine) : State(machine, States::TITLESCREEN), InputHandler(Application::Emitter), m_titleScreen(Application::Emitter) {

	Application::SetCursorIcon(IDC_ARROW);
	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_xaml = m_titleScreen;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

TitleScreenS::~TitleScreenS() {
	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);
}

void TitleScreenS::fixedUpdate() {

}

void TitleScreenS::update() {
	
}

void TitleScreenS::render() {

	// Noesis gui update
	m_ui->Update(Globals::clock.getElapsedTimeSec());
	m_ui->GetRenderer()->UpdateRenderTree();
	m_ui->GetRenderer()->RenderOffscreen();

	// Need to restore the GPU state because noesis changes it	
	Renderer::RestoreGpuState();
	// Render
	m_ui->GetRenderer()->Render();
}

void TitleScreenS::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_ui->MouseMove(event.x, event.y);
}

void TitleScreenS::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonDown(event.x, event.y, Noesis::MouseButton_Left);
}

void TitleScreenS::OnMouseButtonUp(Event::MouseButtonEvent& event) {
	m_ui->MouseButtonUp(event.x, event.y, Noesis::MouseButton_Left);
}

void TitleScreenS::OnMouseWheel(Event::MouseWheelEvent& event) {

}

void TitleScreenS::OnKeyDown(Event::KeyboardEvent& event) {

}

void TitleScreenS::OnKeyUp(Event::KeyboardEvent& event) {

}

void TitleScreenS::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
}

void TitleScreenS::enter() {
	// Subscribe self to inputs
	connectInputs();
}

void TitleScreenS::exit() {
	// Remove input listenner
	disconnectInputs();
}