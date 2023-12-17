#include <NsGui/Button.h>
#include <Event/EventListener.h>

#include "LevelIntro.h"
#include "Application.h"

NS_IMPLEMENT_REFLECTION(LevelIntroBindings) {
	NsMeta<Noesis::TypeId>("LevelIntroBindings");
	NsProp("Title", &LevelIntroBindings::getTitle, &LevelIntroBindings::setTitle);
	NsProp("ImagePath", &LevelIntroBindings::getImagePath, &LevelIntroBindings::setImagePath);
	NsProp("Text", &LevelIntroBindings::getText, &LevelIntroBindings::setText);
}

LevelIntroBindings::LevelIntroBindings() : m_title("-"), m_imagePath(""), m_text("-") {
}

const char* LevelIntroBindings::getTitle() const {
	return m_title.c_str();
}

void LevelIntroBindings::setTitle(const char* value) {
	if (m_title != value) {
		m_title = value;
		OnPropertyChanged("Title");
	}
}

const char* LevelIntroBindings::getImagePath() const {
	return m_imagePath.c_str();
}

void LevelIntroBindings::setImagePath(const char* value) {
	if (m_imagePath != value) {
		m_imagePath = value;
		OnPropertyChanged("ImagePath");
	}
}

const char* LevelIntroBindings::getText() const {
	return m_text.c_str();
}

void LevelIntroBindings::setText(const char* value) {
	if (m_text != value) {
		m_text = value;
		OnPropertyChanged("Text");
	}
}


NS_IMPLEMENT_REFLECTION(LevelIntroGrid) {
	NsMeta<Noesis::TypeId>("LevelIntroGrid");
}

LevelIntroGrid::LevelIntroGrid() {
	m_bindings = *new LevelIntroBindings();
	Initialized() += MakeDelegate(this, &LevelIntroGrid::OnInitialized);
	InitializeComponent();
}

void LevelIntroGrid::InitializeComponent() {
	Noesis::GUI::LoadComponent(this, "level-intro.xaml");
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
	Application::Emitter.publish<evnt::ChangeGameState>(States::LEVEL);
}

void LevelIntroGrid::onExit(Noesis::BaseComponent* sender, const Noesis::RoutedEventArgs& args) {
	Application::Emitter.publish<evnt::ChangeGameState>(States::TITLESCREEN);
}


LevelIntro LevelIntro::s_instance;

LevelIntro& LevelIntro::Get() {
	return s_instance;
}

void LevelIntro::init(EventEmitter& emitter, Progression& progression) {
	m_grid = *new LevelIntroGrid();

	std::string title = "LEVEL " + decimalToRoman(Application::s_Progression.getLevelNumber());
	m_grid->m_bindings->setTitle(title.c_str());
	m_grid->m_bindings->setText(progression.getIntroText());
	m_grid->m_bindings->setImagePath(progression.getIntroImgPath());

	emitter.on<evnt::ProgressionUpdated>([this, &progression](const evnt::ProgressionUpdated & event, EventEmitter & emitter) {
		std::string title = "LEVEL " + this->decimalToRoman(progression.getLevelNumber());
		this->m_grid->m_bindings->setTitle(title.c_str());
		this->m_grid->m_bindings->setText(progression.getIntroText());
		this->m_grid->m_bindings->setImagePath(progression.getIntroImgPath());
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