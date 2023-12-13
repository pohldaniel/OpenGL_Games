#include "level-exit.hpp"

#include <NsGui/Button.h>

#include "Event/change-game-state.hpp"
#include "Event/progression-updated.hpp"
#include "Event/change-game-state.hpp"
#include "States/i-game-state.hpp"

#include "EventListener.h"

NS_IMPLEMENT_REFLECTION(LevelExit) {
	NsMeta<Noesis::TypeId>("LevelExit");
}

LevelExit::LevelExit(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression) {
	m_bindings = *new LevelExitBindings();
	Initialized() += MakeDelegate(this, &LevelExit::OnInitialized);
	InitializeComponent();

	m_bindings->setText(m_progression.getExitText());
	m_emitter.on<evnt::ProgressionUpdated>([this](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {
		this->m_bindings->setText(this->m_progression.getExitText());
	});
}

void LevelExit::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-exit.xaml");
}

void LevelExit::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(m_bindings.GetPtr());
}

bool LevelExit::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onExit);
	NS_CONNECT_EVENT(Noesis::Button, Click, onNextLevel);
	return false;
}

void LevelExit::onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameStateNew>(States::TITLESCREEN);
}

void LevelExit::onNextLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameStateNew>(States::LEVELINTRO);
}

