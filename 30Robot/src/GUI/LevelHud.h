#pragma once

#include <NsGui/Grid.h>
#include <NsGui/IntegrationAPI.h>
#include <NsApp/DelegateCommand.h>
#include <glm/glm.hpp>

#include "Event/EventEmitter.h"
#include "progression.hpp"
#include "level-hud-bindings.hpp"

class LevelHudGrid : public Noesis::Grid {
public:
	LevelHudGrid(EventEmitter& emitter, Progression& progression);

	// Setters
	
	


	// Getters

	// Init 
	void OnInitialized(BaseComponent*, const Noesis::EventArgs&);

	// Events
	bool ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) override;
	void onSelectTowerSlow(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onSelectMirror(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onDeleteEntity(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);
	void onStartWave(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args);

	// Input events
	void OnMouseEnter(const Noesis::MouseEventArgs& e) override;
	void OnMouseLeave(const Noesis::MouseEventArgs& e) override;
	void OnMouseDown(const Noesis::MouseButtonEventArgs& e) override;



	Noesis::Ptr<LevelHudBindings> m_bindings;
	EventEmitter& m_emitter;
	Progression& m_progression;
	std::uint32_t m_lastSelectedEntity;

private:
	NS_DECLARE_REFLECTION(LevelHudGrid, Grid)
};

class LevelHud {

public:

	static LevelHud& Get();
	void init(EventEmitter& emitter, Progression& progression);
	void setSelectedEntity(std::uint32_t id);
	void setOptionsVisibilityTo(bool show);
	void setOptionsPosition(glm::vec2 pos);

	Noesis::Ptr<LevelHudGrid> m_grid;
	

private:
	LevelHud() = default;

	static LevelHud s_instance;
};