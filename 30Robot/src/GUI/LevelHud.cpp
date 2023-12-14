#include <iostream>
#include "LevelHud.h"

#include <NsGui/Button.h>
#include <string>
#include <glm/glm.hpp>

#include "constants.hpp"
#include "States/i-game-state.hpp"
#include "Event/Interactions/construct-selection.hpp"
#include "Event/Interactions/delete-entity.hpp"
#include "Event/progression-updated.hpp"
#include "Event/wave-updated.hpp"
#include "System/wave-system.hpp"
#include "Application.h"

NS_IMPLEMENT_REFLECTION(LevelHudGrid) {
	NsMeta<Noesis::TypeId>("LevelHudGrid");
}

LevelHudGrid::LevelHudGrid(EventEmitter& emitter, Progression& progression) : m_emitter(emitter), m_progression(progression){
	m_bindings = *new LevelHudBindings();
	Initialized() += MakeDelegate(this, &LevelHudGrid::OnInitialized);	
	InitializeComponent();
}

void LevelHudGrid::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-hud.xaml");
}

bool LevelHudGrid::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onSelectTowerSlow);
	NS_CONNECT_EVENT(Noesis::Button, Click, onSelectMirror);
	NS_CONNECT_EVENT(Noesis::Button, Click, onDeleteEntity);
	NS_CONNECT_EVENT(Noesis::Button, Click, onStartWave);
	return false;
}

void LevelHudGrid::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(m_bindings.GetPtr());
}

void LevelHudGrid::OnMouseEnter(const Noesis::MouseEventArgs& e) {
	m_emitter.focus = FocusMode::HUD;
}

void LevelHudGrid::OnMouseLeave(const Noesis::MouseEventArgs& e) {
	m_emitter.focus = FocusMode::GAME;
}

void LevelHudGrid::OnMouseDown(const Noesis::MouseButtonEventArgs& e) {
}

void LevelHudGrid::onSelectTowerSlow(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	if (m_progression.getSlowNumbers() > 0) {
		m_emitter.publish<evnt::ConstructSelection>(ConstructibleType::TOWER_SLOW);
	}
}

void LevelHudGrid::onSelectMirror(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	if (m_progression.getMirrorNumbers() > 0) {
		m_emitter.publish<evnt::ConstructSelection>(ConstructibleType::MIRROR_BASIC);
	}
}

void LevelHudGrid::onDeleteEntity(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::DeleteEntity>(m_lastSelectedEntity);
}

void LevelHudGrid::onStartWave(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	m_emitter.publish<evnt::StartWave>();
	m_bindings->setStartWaveBtnPosY(-500);
}

/* ------------------------ SETTERS ----------------------- */
LevelHud LevelHud::s_instance;

LevelHud& LevelHud::Get() {
	return s_instance;
}

void LevelHud::init(EventEmitter& emitter, Progression& progression) {
	m_grid = *new LevelHudGrid(emitter, progression);
	
	m_grid->m_bindings->setLife(std::to_string(progression.getLife()).c_str());
	m_grid->m_bindings->setMirrorNumber(std::to_string(progression.getMirrorNumbers()).c_str());
	m_grid->m_bindings->setSlowNumber(std::to_string(progression.getSlowNumbers()).c_str());


	emitter.on<evnt::ProgressionUpdated>([this, &progression](const evnt::ProgressionUpdated& event, EventEmitter& emitter) {
		this->m_grid->m_bindings->setLife(std::to_string(progression.getLife()).c_str());
		this->m_grid->m_bindings->setMirrorNumber(std::to_string(progression.getMirrorNumbers()).c_str());
		this->m_grid->m_bindings->setSlowNumber(std::to_string(progression.getSlowNumbers()).c_str());
	});

	emitter.on<evnt::WaveUpdated>([this](const evnt::WaveUpdated& event, EventEmitter& emitter) {
		switch (event.state) {
		case WaveState::NOT_STARTED:
			this->m_grid->m_bindings->setTimer("-");
			this->m_grid->m_bindings->setStartWaveBtnPosY(0);
			break;

		case WaveState::PENDING:
			this->m_grid->m_bindings->setTimer("Get ready...");
			break;

		case WaveState::DURING:
			this->m_grid->m_bindings->setTimer(std::to_string(event.timer).c_str());
			break;

		case WaveState::DONE:
			this->m_grid->m_bindings->setTimer("Kill what remains");
			break;

		default:
			break;
		}
	});
}

void LevelHud::setSelectedEntity(std::uint32_t id) {
	m_grid->m_lastSelectedEntity = id;
}

void LevelHud::setOptionsVisibilityTo(bool show) {
	if (show) {
		m_grid->m_bindings->setOptionsVisibility("Visible");
	}
	else {
		m_grid->m_bindings->setOptionsVisibility("Collapsed");
	}
}

void LevelHud::setOptionsPosition(glm::vec2 pos) {
	// Invert pos Y to match 0.0 on the top
	pos.y = WIN_HEIGHT - pos.y;

	// Make the pivot point bottom left with an offset corresponding to menu size
	pos.y -= 55;
	pos.x += 25;

	m_grid->m_bindings->setOptionsVisibility("Visible");
	m_grid->m_bindings->setOptionsPosX(pos.x);
	m_grid->m_bindings->setOptionsPosY(pos.y);
}