#include "GameOver.h"

#include <NsGui/Button.h>
#include <Event/EventListener.h>

NS_IMPLEMENT_REFLECTION(GameOverGrid) {
	NsMeta<Noesis::TypeId>("GameOverGrid");
}

GameOverGrid::GameOverGrid(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression) {
	InitializeComponent();
}

void GameOverGrid::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "game-over.xaml");
}

bool GameOverGrid::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onExit);
	NS_CONNECT_EVENT(Noesis::Button, Click, onRestart);
	return false;
}

void GameOverGrid::onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(States::TITLESCREEN);
}

void GameOverGrid::onRestart(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::ChangeGameState>(States::LEVELINTRO);
}

GameOver GameOver::s_instance;

GameOver& GameOver::Get() {
	return s_instance;
}

void GameOver::init(EventEmitter& emitter, Progression& progression) {
	m_grid = *new GameOverGrid(emitter, progression);
}