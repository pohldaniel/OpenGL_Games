#pragma once

#include <NsGui/IntegrationAPI.h>
#include <NsGui/Grid.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>
#include <Event/EventEmitter.h>

#include "Progression.h"

class LevelExitBindings : public NoesisApp::NotifyPropertyChangedBase {
public:
	LevelExitBindings();

	const char* getText() const;
	void setText(const char* value);

private:
	NS_DECLARE_REFLECTION(LevelExitBindings, NotifyPropertyChangedBase)

		NsString m_text;
};


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