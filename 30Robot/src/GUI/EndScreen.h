#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>
#include <Event/EventEmitter.h>

#include "Progression.h"

class EndScreenGrid : public Noesis::Grid {
public:
	EndScreenGrid(EventEmitter& emitter);

private:
	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;

	// Init 
	void InitializeComponent();

private:
	NS_DECLARE_REFLECTION(EndScreenGrid, Grid);
	EventEmitter& m_emitter;
};

class EndScreen {

public:

	static EndScreen& Get();
	void init(EventEmitter& emitter, Progression& progression);

	Noesis::Ptr<EndScreenGrid> m_grid;

private:
	EndScreen() = default;

	static EndScreen s_instance;
};
