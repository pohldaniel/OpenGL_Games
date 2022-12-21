#include "Options.h"
#include "MainMenu.h"
#include "SelectionBox.h"
#include "CheckBox.h"
#include "SeekerBar.h"

Options::Options(StateMachine& machine) : State(machine, CurrentState::OPTIONS) {
	m_dialog = Dialog(0, 0, 370, 150);
	m_dialog.setPosition(0, 0);

	m_dialog.setAutoresize();
	m_dialog.setCenterOnScreen();

	m_dialog.addChildWidget(64, 240, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Options")));
	m_dialog.addChildWidget(64, 190, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Resolution: ")));
	m_dialog.addChildWidget(64, 160, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Fullscreen: ")));
	m_dialog.addChildWidget(64, 130, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Sound Volume: ")));	
	m_dialog.addChildWidget(64, 100, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Music Volume: ")));
	m_dialog.addChildWidget(64, 70, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Back")));
	

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[0])->setDefaultColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[0])->setHoverColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[5])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new MainMenu(m_machine));
	});

	std::auto_ptr<CheckBox> checkBox(new CheckBox(m_checked));

	checkBox->setOnChecked([&]() {
		Application::SetFullScreen(m_screenModes[m_selected]);
	});

	checkBox->setOnUnchecked([&]() {
		Application::ResetFullScreen();
	});

	m_dialog.addChildWidget(335, 160, std::auto_ptr<Widget>(checkBox));

	std::auto_ptr<SelectionBox> optionsFrameResolutionSelection(new SelectionBox(Globals::fontManager.get("verdana_20"), Globals::fontManager.get("verdana_10"), m_selected));
	optionsFrameResolutionSelection->setBaseColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	optionsFrameResolutionSelection->setSelectColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	
	optionsFrameResolutionSelection->setOnSelected([&](int selected) {
		if(m_checked)
			Application::SetFullScreen(m_screenModes[m_selected]);
	});

	Application::GetScreenMode(m_screenModes);
	std::vector<std::string> resTexts;

	m_selected = 0;
	for (size_t curResNr = 0; curResNr < m_screenModes.size(); ++curResNr) {
		DEVMODE& screen = m_screenModes[curResNr];
		std::ostringstream oss;
		oss << screen.dmPelsWidth << "x" << screen.dmPelsHeight;
		resTexts.push_back(oss.str());
	}

	optionsFrameResolutionSelection->setEntries(resTexts, m_selected);

	std::auto_ptr<SeekerBar> seekerBar1(new SeekerBar(Globals::soundVolume));
	seekerBar1->setOnClicked([&]() {
		Globals::soundManager.get("effect").setVolume(Globals::soundVolume);
		Globals::soundManager.get("player").setVolume(Globals::soundVolume);
		Globals::soundManager.get("player").setVolumeChannel(10, Globals::soundVolume);
	});
	
	std::auto_ptr<SeekerBar> seekerBar2(new SeekerBar(Globals::musicVolume));
	seekerBar2->setOnClicked([&]() {
		Globals::musicManager.get("background").setVolume(Globals::musicVolume);
	});

	m_dialog.addChildWidget(260, 130, std::auto_ptr<Widget>(seekerBar1.release()));
	m_dialog.addChildWidget(260, 100, std::auto_ptr<Widget>(seekerBar2.release()));


	m_dialog.addChildWidget(260, 190, std::auto_ptr<Widget>(optionsFrameResolutionSelection.release()));

	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("font")->getAtlas(), true, 0);
	TextureManager::BindTexture(TextureManager::GetTextureAtlas("dialog"), true, 2);
	Globals::musicManager.get("background").play("res/music/Early_Dawn_Simple.ogg");
}


Options::~Options() {}

void Options::fixedUpdate() {}

void Options::update() {
	m_dialog.processInput();
}

void Options::render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_dialog.draw();
	TextureManager::DrawBuffer();
}

void Options::resize(int deltaW, int deltaH) {
	m_dialog.applyLayout();
}