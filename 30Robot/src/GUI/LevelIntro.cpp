#include "LevelIntro.h"

#include <NsGui/Button.h>

#include "Event/change-game-state.hpp"
#include "Event/progression-updated.hpp"

#include "EventListener.h"
#include "Application.h"

#include <iostream>

NS_IMPLEMENT_REFLECTION(LevelIntroGrid) {
	NsMeta<Noesis::TypeId>("LevelIntroGrid");
}

LevelIntroGrid::LevelIntroGrid() {
	m_bindings = *new LevelIntroBindings();
	Initialized() += MakeDelegate(this, &LevelIntroGrid::OnInitialized);
}

bool LevelIntroGrid::ConnectEvent(Noesis::BaseComponent* source, const char* event, const char* handler) {
	NS_CONNECT_EVENT(Noesis::Button, Click, onStartLevel);
	NS_CONNECT_EVENT(Noesis::Button, Click, onExit);
	return false;
}

void LevelIntroGrid::OnInitialized(BaseComponent*, const Noesis::EventArgs&) {
	SetDataContext(m_bindings.GetPtr());
}

void LevelIntroGrid::onStartLevel(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	Application::Emitter.publish<evnt::ChangeGameStateNew>(States::LEVEL);
}

void LevelIntroGrid::onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	Application::Emitter.publish<evnt::ChangeGameStateNew>(States::TITLESCREEN);
}


LevelIntro LevelIntro::s_instance;

LevelIntro& LevelIntro::Get() {
	return s_instance;
}

void LevelIntro::init() {
	m_grid = *new LevelIntroGrid();
	Noesis::GUI::LoadComponent(m_grid, "level-intro.xaml");

	std::string title = "LEVEL " + decimalToRoman(Application::s_Progression.getLevelNumber());
	m_grid->m_bindings->setTitle(title.c_str());
	m_grid->m_bindings->setText(Application::s_Progression.getIntroText());
	m_grid->m_bindings->setImagePath(Application::s_Progression.getIntroImgPath());

	/*m_emitter.on<evnt::ProgressionUpdated>([this, &m_progression = m_progression, m_bindings = m_bindings](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {
		std::string title = "LEVEL " + this->decimalToRoman(m_progression.getLevelNumber());
		m_bindings->setTitle(title.c_str());
		m_bindings->setText(m_progression.getIntroText());
		m_bindings->setImagePath(m_progression.getIntroImgPath());
	});*/

	Application::Emitter.on<evnt::ProgressionUpdated>([this](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {
		std::string title = "LEVEL " + this->decimalToRoman(Application::s_Progression.getLevelNumber());
		this->m_grid->m_bindings->setTitle(title.c_str());
		this->m_grid->m_bindings->setText(Application::s_Progression.getIntroText());
		this->m_grid->m_bindings->setImagePath(Application::s_Progression.getIntroImgPath());
	});
}

std::string LevelIntro::decimalToRoman(int num) {
	int decimal[] = { 1000,900,500,400,100,90,50,40,10,9,5,4,1 }; //base values
	char* symbol[] = { "M","CM","D","CD","C","XC","L","XL","X","IX","V","IV","I" };  //roman symbols
	int i = 0;
	std::string result = "";

	while (num) { //repeat process until num is not 0
		while (num / decimal[i]) {  //first base value that divides num is largest base value
			result += symbol[i];    //print roman symbol equivalent to largest base value
			num -= decimal[i];  //subtract largest base value from num
		}
		i++;    //move to next base value to divide num
	}
	return result;
}