#include <NsGui/IntegrationAPI.h>
#include <States/LevelIntroS.h>
#include <GUI/TitleScreen.h>

#include "TitleScreenS.h"
#include "Application.h"
#include "Globals.h"
#include "Renderer.h"

TitleScreenS::TitleScreenS(StateMachine& machine) : State(machine, States::TITLESCREEN) {

	EventDispatcher::AddKeyboardListener(this);
	EventDispatcher::AddMouseListener(this);

	m_xaml = TitleScreen::Get().m_grid;
	m_ui = Noesis::GUI::CreateView(m_xaml).GiveOwnership();
	m_ui->SetIsPPAAEnabled(true);
	m_ui->GetRenderer()->Init(Application::NoesisDevice);
	m_ui->SetSize(Application::Width, Application::Height);
}

TitleScreenS::~TitleScreenS() {
	std::cout << "Destructor Title: " << std::endl;

	EventDispatcher::RemoveKeyboardListener(this);
	EventDispatcher::RemoveMouseListener(this);

	m_ui->GetRenderer()->Shutdown();
	delete m_ui;
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

void TitleScreenS::resize(int deltaW, int deltaH) {
	m_ui->SetSize(Application::Width, Application::Height);
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

void TitleScreenS::OnStateChange(States states) {
	m_isRunning = false;
	m_machine.addStateAtBottom(new LevelIntroS(m_machine));
}

void TitleScreenS::OnApplicationQuit() {
	m_isRunning = false;
}