#pragma once

#include <glm/glm.hpp>

#include <NsGui/Grid.h>
#include <NsGui/Storyboard.h>
#include <NsGui/IntegrationAPI.h>

#include <Event/EventEmitter.h>
#include <Event/EventListener.h>

#include "progression.hpp"

class TitleScreenGrid: public Noesis::Grid {

	friend class TitleScreen;

public:

	TitleScreenGrid(EventEmitter& emitter);

private:

	void InitializeComponent();

    // Events
    bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onStartGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onQuitGame(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

private:

	NS_DECLARE_REFLECTION(TitleScreenGrid, Grid);
	EventEmitter& m_emitter;

	Noesis::Storyboard* m_storyBoard;
};

class TitleScreen {

public:

	static TitleScreen& Get();
	void init(EventEmitter& emitter, Progression& progression);
	void clearAnimations();

	Noesis::Ptr<TitleScreenGrid> m_grid;

private:
	TitleScreen() = default;

	static TitleScreen s_instance;
};