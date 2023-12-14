#include "EndScreen.h"

#include <NsGui/Button.h>

NS_IMPLEMENT_REFLECTION(EndScreenGrid) {
	NsMeta<Noesis::TypeId>("EndScreenGrid");
}

EndScreenGrid::EndScreenGrid(EventEmitter& emitter) : m_emitter(emitter) {
	InitializeComponent();
}

void EndScreenGrid::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "end-screen.xaml");
}

bool EndScreenGrid::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	return false;
}

EndScreen EndScreen::s_instance;

EndScreen& EndScreen::Get() {
	return s_instance;
}

void EndScreen::init(EventEmitter& emitter, Progression& progression) {
	m_grid = *new EndScreenGrid(emitter);
}