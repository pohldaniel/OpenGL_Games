#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>
#include <Event/EventEmitter.h>

#include "Progression.h"

class GameOverGrid : public Noesis::Grid {
public:
	GameOverGrid(EventEmitter& emitter, Progression& progression);

private:
	// Init 
	void InitializeComponent();

	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onRestart(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

private:
	NS_DECLARE_REFLECTION(GameOverGrid, Grid);
	EventEmitter& m_emitter;
	Progression& m_progression;
};

class GameOver {

public:

	static GameOver& Get();
	void init(EventEmitter& emitter, Progression& progression);

	Noesis::Ptr<GameOverGrid> m_grid;

private:
	GameOver() = default;

	static GameOver s_instance;
};