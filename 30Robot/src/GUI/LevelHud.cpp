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


NS_IMPLEMENT_REFLECTION(LevelHudBindings) {
	NsMeta<Noesis::TypeId>("LevelHudBindings");
	NsProp("OptionsPosX", &LevelHudBindings::getOptionsPosX, &LevelHudBindings::setOptionsPosX);
	NsProp("OptionsPosY", &LevelHudBindings::getOptionsPosY, &LevelHudBindings::setOptionsPosY);
	NsProp("OptionsVisibility", &LevelHudBindings::getOptionsVisibility, &LevelHudBindings::setOptionsVisibility);
	NsProp("StartWaveBtnVisibility", &LevelHudBindings::getStartWaveBtnVisibility, &LevelHudBindings::setStartWaveBtnVisibility);
	NsProp("StartWaveBtnPosY", &LevelHudBindings::getStartWaveBtnPosY, &LevelHudBindings::setStartWaveBtnPosY);
	NsProp("Timer", &LevelHudBindings::getTimer, &LevelHudBindings::setTimer);
	NsProp("Life", &LevelHudBindings::getLife, &LevelHudBindings::setLife);
	NsProp("MirrorNumber", &LevelHudBindings::getMirrorNumber, &LevelHudBindings::setMirrorNumber);
	NsProp("SlowNumber", &LevelHudBindings::getSlowNumber, &LevelHudBindings::setSlowNumber);
}

LevelHudBindings::LevelHudBindings()
	: m_optionsVisibility("Visible"), m_optionsPosX(0.0f), m_optionsPosY(-500.0f),
	m_startWaveBtnVisibility("Visible"), m_startWaveBtnPosY(0.0f),
	m_timer("-"), m_life("-"), m_mirrorNumber("-"), m_slowNumber("-")
{
}

/* ---------------- Flying option menu -------------- */

void LevelHudBindings::setOptionsPosX(float pos) {
	if (m_optionsPosX != pos) {
		m_optionsPosX = pos;
		OnPropertyChanged("OptionsPosX");
	}
}

float LevelHudBindings::getOptionsPosX() const {
	return m_optionsPosX;
}

void LevelHudBindings::setOptionsPosY(float pos) {
	if (m_optionsPosY != pos) {
		m_optionsPosY = pos;
		OnPropertyChanged("OptionsPosY");
	}
}

float LevelHudBindings::getOptionsPosY() const {
	return m_optionsPosY;
}


const char* LevelHudBindings::getOptionsVisibility() const {
	return m_optionsVisibility.c_str();
}

void LevelHudBindings::setOptionsVisibility(const char* value) {
	if (m_optionsVisibility != value) {
		m_optionsVisibility = value;
		// OnPropertyChanged("Visibitity");

		// Temp fix because does not work at runtime
		if (Noesis::String::Equals("Collapsed", value)) {
			m_optionsPosY = -500;
			OnPropertyChanged("OptionsPosY");
		}
	}
}

/* ------------------- TopBar ---------------------- */
const char* LevelHudBindings::getStartWaveBtnVisibility() const {
	return m_startWaveBtnVisibility.c_str();
}

void LevelHudBindings::setStartWaveBtnVisibility(const char* value) {
	if (m_startWaveBtnVisibility != value) {
		m_startWaveBtnVisibility = value;
		// OnPropertyChanged("Visibitity");

		// Temp fix because does not work at runtime
		if (Noesis::String::Equals("Collapsed", value)) {
			m_startWaveBtnPosY = 500;
			OnPropertyChanged("StartWaveBtnVisibility");
		}
	}
}

void LevelHudBindings::setStartWaveBtnPosY(float pos) {
	if (m_startWaveBtnPosY != pos) {
		m_startWaveBtnPosY = pos;
		OnPropertyChanged("StartWaveBtnPosY");
	}
}

float LevelHudBindings::getStartWaveBtnPosY() const {
	return m_startWaveBtnPosY;
}

const char* LevelHudBindings::getTimer() const {
	return m_timer.c_str();
}

void LevelHudBindings::setTimer(const char* value) {
	if (m_timer != value) {
		m_timer = value;
		OnPropertyChanged("Timer");
	}
}

const char* LevelHudBindings::getLife() const {
	return m_life.c_str();
}

void LevelHudBindings::setLife(const char* value) {
	if (m_life != value) {
		m_life = value;
		OnPropertyChanged("Life");
	}
}

/* ------------ BottomBar ------------------ */

const char* LevelHudBindings::getMirrorNumber() const {
	return m_mirrorNumber.c_str();
}

void LevelHudBindings::setMirrorNumber(const char* value) {
	if (m_mirrorNumber != value) {
		m_mirrorNumber = value;
		OnPropertyChanged("MirrorNumber");
	}
}

const char* LevelHudBindings::getSlowNumber() const {
	return m_slowNumber.c_str();
}

void LevelHudBindings::setSlowNumber(const char* value) {
	if (m_slowNumber != value) {
		m_slowNumber = value;
		OnPropertyChanged("SlowNumber");
	}
}

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

void LevelHud::reset() {
	m_grid->m_bindings->setLife("6");
	m_grid->m_bindings->setStartWaveBtnPosY(0);
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