#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "Event/EventEmitter.h"
#include "progression.hpp"
#include "level-exit-bindings.hpp"

class LevelExitGrid : public Noesis::Grid {

public:
	LevelExitGrid(EventEmitter& emitter, Progression& progression);

	// Init 
	void InitializeComponent();
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onNextLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	
	Noesis::Ptr<LevelExitBindings> m_bindings;

private:
	NS_DECLARE_REFLECTION(LevelExitGrid, Grid);
	EventEmitter& m_emitter;
	Progression& m_progression;
};

class LevelExit {

public:

	static LevelExit& Get();
	void init(EventEmitter& emitter, Progression& progression);

	Noesis::Ptr<LevelExitGrid> m_grid;


private:

	LevelExit() = default;

	static LevelExit s_instance;
};