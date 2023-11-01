#include "Options.h"
#include "MainMenu.h"
#include "SelectionBox.h"
#include "CheckBox.h"
#include "SeekerBar.h"

Options::Options(StateMachine& machine) : State(machine, CurrentState::OPTIONS) {
	EventDispatcher::AddMouseListener(this);
	
	m_dialog = Dialog(0, 0, 370, 150);
	m_dialog.setPosition(0, 0);

	m_dialog.setAutoresize();
	m_dialog.setCenterOnScreen();

	m_dialog.addChildWidget(64, 240, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Options")));
	m_dialog.addChildWidget(64, 190, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Resolution: ")));
	m_dialog.addChildWidget(64, 160, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Apply Displaymode: ")));
	m_dialog.addChildWidget(64, 130, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Sound Volume: ")));	
	m_dialog.addChildWidget(64, 100, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Music Volume: ")));
	m_dialog.addChildWidget(64, 70, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Back")));
	
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[0])->setDefaultColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[0])->setHoverColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[5])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtBottom(new MainMenu(m_machine));
	});

	std::auto_ptr<CheckBox> checkBox(new CheckBox(Globals::applyDisplaymode));

	checkBox->setOnChecked([&]() {
		Application::SetDisplayMode(m_screenModes[m_selected]);
	});

	checkBox->setOnUnchecked([&]() {
		Application::ResetDisplayMode();
	});

	m_dialog.addChildWidget(335, 160, std::auto_ptr<Widget>(checkBox));

	std::auto_ptr<SelectionBox> optionsFrameResolutionSelection(new SelectionBox(Globals::fontManager.get("verdana_20"), Globals::fontManager.get("verdana_10"), m_selected));
	optionsFrameResolutionSelection->setBaseColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	optionsFrameResolutionSelection->setSelectColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));
	
	optionsFrameResolutionSelection->setOnSelected([&](int selected) {
		Globals::width = m_screenModes[selected].dmPelsWidth;
		Globals::height = m_screenModes[selected].dmPelsHeight;
		if(Globals::applyDisplaymode && m_selected != selected)
			Application::SetDisplayMode(m_screenModes[selected]);
	});

	Application::GetScreenMode(m_screenModes);
	std::vector<std::string> resTexts;

	m_selected = 0;
	for (size_t curResNr = 0; curResNr < m_screenModes.size(); ++curResNr) {
		DEVMODE& screen = m_screenModes[curResNr];
		std::ostringstream oss;
		oss << screen.dmPelsWidth << "x" << screen.dmPelsHeight;
		resTexts.push_back(oss.str());

		if (Globals::width == static_cast<unsigned int>(screen.dmPelsWidth) && Globals::height == static_cast<unsigned int>(screen.dmPelsHeight)) {			
			m_selected = static_cast<int>(curResNr);
		}
	}

	optionsFrameResolutionSelection->setEntries(resTexts, m_selected);

	std::auto_ptr<SeekerBar> seekerBar1(new SeekerBar(Globals::soundVolume));
	seekerBar1->setOnClicked([&]() {
		Globals::soundManager.get("effect").setVolume(Globals::soundVolume);
		Globals::soundManager.get("player").setVolume(Globals::soundVolume);
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
	//Globals::musicManager.get("background").play("res/music/Early_Dawn_Simple.ogg");
}


Options::~Options() {
	EventDispatcher::RemoveMouseListener(this);
}

void Options::fixedUpdate() {}

void Options::update() {}

void Options::render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_dialog.draw();
	TextureManager::DrawBuffer();
}

void Options::resize(int deltaW, int deltaH) {
	m_dialog.applyLayout();
}

void Options::OnMouseMotion(Event::MouseMoveEvent& event) {
	m_dialog.processInput(event.x, ViewPort::Get().getHeight() - event.y);
}

void Options::OnMouseButtonDown(Event::MouseButtonEvent& event) {
	m_dialog.processInput(event.x, ViewPort::Get().getHeight() - event.y, event.button);
}