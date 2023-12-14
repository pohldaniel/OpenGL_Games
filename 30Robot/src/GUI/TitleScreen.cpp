#include <iostream>
#include "TitleScreen.h"

#include <NsGui/Button.h>
#include "Event/change-game-state.hpp"
#include "EventListener.h"
#include "States/i-game-state.hpp"

NS_IMPLEMENT_REFLECTION(TitleScreenGrid) {
	NsMeta<Noesis::TypeId>("TitleScreenGrid");
}

TitleScreenGrid::TitleScreenGrid(EventEmitter& emitter) : m_emitter(emitter) {
	InitializeComponent();
}

void TitleScreenGrid::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "title-screen.xaml");
}

bool TitleScreenGrid::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onStartGame);
	NS_CONNECT_EVENT(Noesis::Button, Click, onQuitGame);
    return false;
}

void TitleScreenGrid::onStartGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameStateNew>(States::LEVELINTRO);
}

void TitleScreenGrid::onQuitGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ApplicationExit>();
}

TitleScreen TitleScreen::s_instance;

TitleScreen& TitleScreen::Get() {
	return s_instance;
}

void TitleScreen::init(EventEmitter& emitter, Progression& progression) {
	m_grid = *new TitleScreenGrid(emitter);
}