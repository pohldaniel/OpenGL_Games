#include "configuredframes.h"
#include "Label.h"
#include "Fontcache.h"

namespace ConfiguredFrames{

	void fillMainMenuFrame(ConfigurableFrame* mainMenuFrame){
		// setup main menu frame
		mainMenuFrame->setAutoresize();
		mainMenuFrame->setCenteringLayout();
		mainMenuFrame->setCenterOnScreen();
		std::auto_ptr<Label> quitLabel(new Label(FontCache::getFontFromCache("res/verdana.ttf", 20), "Quit Game"));
		quitLabel->setBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
		quitLabel->setSelectColor(1.0f, 1.0f, 0.0f, 1.0f);
		std::auto_ptr<Label> optionsLabel(new Label(FontCache::getFontFromCache("res/verdana.ttf", 20), "Options"));
		optionsLabel->setBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
		optionsLabel->setSelectColor(1.0f, 1.0f, 0.0f, 1.0f);
		std::auto_ptr<Label> newGameLabel(new Label(FontCache::getFontFromCache("res/verdana.ttf", 20), "New Game"));
		newGameLabel->setBaseColor(1.0f, 1.0f, 1.0f, 1.0f);
		newGameLabel->setSelectColor(1.0f, 1.0f, 0.0f, 1.0f);

		//optionsLabel->setOnClicked(new ToggleFrameFunction(mainMenuFrame, optionsFrame.get()));
		//newGameLabel->setOnClicked(new ToggleFrameFunction(mainMenuFrame, chooseClassFrame.get()));
		//quitLabel->setOnClicked(new QuitGameFunction());

		mainMenuFrame->addChildFrame(0, 0, std::auto_ptr<FramesBase>(quitLabel.release()));
		mainMenuFrame->addChildFrame(0, 10, std::auto_ptr<FramesBase>(optionsLabel.release()));
		mainMenuFrame->addChildFrame(0, 30, std::auto_ptr<FramesBase>(newGameLabel.release()));
	}
}