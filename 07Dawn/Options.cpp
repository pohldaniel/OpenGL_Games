#include "Options.h"
#include "MainMenu.h"
#include "SelectionBox.h"
#include "CheckBox.h"

Options::Options(StateMachine& machine) : State(machine, CurrentState::OPTIONS) {
	m_dialog = Dialog(0, 0, 270, 120);
	m_dialog.setPosition(0, 0);

	m_dialog.setAutoresize();
	m_dialog.setCenterOnScreen();

	m_dialog.addChildWidget(64, 80, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Back")));	
	m_dialog.addChildWidget(64, 110, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Fullscreen: ")));
	m_dialog.addChildWidget(64, 140, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Resolution: ")));
	m_dialog.addChildWidget(64, 170, std::auto_ptr<Widget>(new Label(Globals::fontManager.get("verdana_20"), "Options")));

	dynamic_cast<Label*>(m_dialog.getChildWidgets()[3])->setDefaultColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[3])->setHoverColor(Vector4f(1.0f, 0.0f, 0.0f, 1.0f));
	dynamic_cast<Label*>(m_dialog.getChildWidgets()[0])->setFunction([&]() {
		m_isRunning = false;
		m_machine.addStateAtTop(new MainMenu(m_machine));
	});

	std::auto_ptr<CheckBox> checkBox(new CheckBox());

	checkBox->setOnChecked([&]() {
		Application::SetFullScreen(m_screenModes[m_selected]);
	});

	checkBox->setOnUnchecked([&]() {
		Application::ResetFullScreen();
	});

	m_dialog.addChildWidget(230, 110, std::auto_ptr<Widget>(checkBox));

	std::auto_ptr<SelectionBox> optionsFrameResolutionSelection(new SelectionBox(Globals::fontManager.get("verdana_20"), Globals::fontManager.get("verdana_10")));
	optionsFrameResolutionSelection->setBaseColor(Vector4f(1.0f, 1.0f, 1.0f, 1.0f));
	optionsFrameResolutionSelection->setSelectColor(Vector4f(1.0f, 1.0f, 0.0f, 1.0f));

	Application::GetScreenMode(m_screenModes);
	std::vector<std::string> resTexts;
	int selected = 0;
	for (size_t curResNr = 0; curResNr < m_screenModes.size(); ++curResNr) {
		DEVMODE& screen = m_screenModes[curResNr];
		std::ostringstream oss;
		oss << screen.dmPelsWidth << "x" << screen.dmPelsHeight;
		resTexts.push_back(oss.str());
	}

	optionsFrameResolutionSelection->setEntries(resTexts, selected);
	m_selected = selected;

	m_dialog.addChildWidget(230, 140, std::auto_ptr<Widget>(optionsFrameResolutionSelection.release()));

	TextureManager::BindTexture(Globals::spritesheetManager.getAssetPointer("font")->getAtlas(), true, 0);
}


Options::~Options() {}

void Options::fixedUpdate() {}

void Options::update() {
	m_dialog.processInput();
	m_selected = dynamic_cast<SelectionBox*>(m_dialog.getChildWidgets()[5])->getSelected();
}

void Options::render() {
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);
	m_dialog.draw();
}

void Options::resize(int deltaW, int deltaH) {
	m_dialog.applyLayout();
}