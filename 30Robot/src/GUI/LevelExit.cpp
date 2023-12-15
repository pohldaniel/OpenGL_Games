#include <NsGui/Button.h>

#include "LevelExit.h"
#include "Event/progression-updated.hpp"
#include "EventListener.h"

NS_IMPLEMENT_REFLECTION(LevelExitBindings) {
	NsMeta<Noesis::TypeId>("LevelExitBindings");
	NsProp("Text", &LevelExitBindings::getText, &LevelExitBindings::setText);
}

LevelExitBindings::LevelExitBindings() : m_text("") {
}

const char* LevelExitBindings::getText() const {
	return m_text.c_str();
}

void LevelExitBindings::setText(const char* value) {
	if (m_text != value) {
		m_text = value;
		OnPropertyChanged("Text");
	}
}

NS_IMPLEMENT_REFLECTION(LevelExitGrid) {
	NsMeta<Noesis::TypeId>("LevelExitGrid");
}

LevelExitGrid::LevelExitGrid(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression) {
	m_bindings = *new LevelExitBindings();
	Initialized() += MakeDelegate(this, &LevelExitGrid::OnInitialized);
	InitializeComponent();
}

void LevelExitGrid::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-exit.xaml");
}

void LevelExitGrid::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(m_bindings.GetPtr());
}

bool LevelExitGrid::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onExit);
	NS_CONNECT_EVENT(Noesis::Button, Click, onNextLevel);
	return false;
}

void LevelExitGrid::onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameStateNew>(States::TITLESCREEN);
}

void LevelExitGrid::onNextLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameStateNew>(States::LEVELINTRO);
}

LevelExit LevelExit::s_instance;

LevelExit& LevelExit::Get() {
	return s_instance;
}

void LevelExit::init(EventEmitter& emitter, Progression& progression) {
	m_grid = *new LevelExitGrid(emitter, progression);
	

	this->m_grid->m_bindings->setText(progression.getExitText());
	emitter.on<evnt::ProgressionUpdated>([this, &progression](const evnt::ProgressionUpdated& event, EventEmitter& emitter) mutable {
		this->m_grid->m_bindings->setText(progression.getExitText());
	});
}
