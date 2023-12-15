#pragma once

#include <NsGui/IntegrationAPI.h>
#include <NsGui/Grid.h>
#include <NsApp/DelegateCommand.h>
#include <NsApp/NotifyPropertyChangedBase.h>

#include "Event/EventEmitter.h"
#include "progression.hpp"

class LevelHudBindings : public NoesisApp::NotifyPropertyChangedBase {

public:
	LevelHudBindings();

	// Flying option menu
	const char* getOptionsVisibility() const;
	void setOptionsVisibility(const char* value);

	void setOptionsPosX(float pos);
	float getOptionsPosX() const;

	void setOptionsPosY(float pos);
	float getOptionsPosY() const;

	// Start wave btn
	const char* getStartWaveBtnVisibility() const;
	void setStartWaveBtnVisibility(const char* value);

	void setStartWaveBtnPosY(float pos);
	float getStartWaveBtnPosY() const;

	// TopBar
	const char* getTimer() const;
	void setTimer(const char* value);

	const char* getLife() const;
	void setLife(const char* value);

	// BottomBar
	const char* getMirrorNumber() const;
	void setMirrorNumber(const char* value);

	const char* getSlowNumber() const;
	void setSlowNumber(const char* value);

private:
	NS_DECLARE_REFLECTION(LevelHudBindings, NotifyPropertyChangedBase)

	float m_optionsPosX;
	float m_optionsPosY;
	NsString m_optionsVisibility;
	NsString m_startWaveBtnVisibility;
	float m_startWaveBtnPosY;
	NsString m_timer;
	NsString m_life;
	NsString m_mirrorNumber;
	NsString m_slowNumber;
};

class LevelHudGrid : public Noesis::Grid {

public:
	LevelHudGrid(EventEmitter& emitter, Progression& progression);

	// Init 
	void InitializeComponent();
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
	void reset();

	void setSelectedEntity(std::uint32_t id);
	void setOptionsVisibilityTo(bool show);
	void setOptionsPosition(glm::vec2 pos);

	Noesis::Ptr<LevelHudGrid> m_grid;
	

private:
	LevelHud() = default;

	static LevelHud s_instance;
};