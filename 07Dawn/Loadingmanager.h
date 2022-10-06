#pragma once

#include <string>
#include "thread/Thread.h"

class LoadingManager : public CThread {

public:
	void setProgress(float newProgress) {
		progress = newProgress;
	}

	float getProgress() {
		return progress;
	}

	bool isFinished() {
		bool result = false;
		accessMutex.Lock();
		result = finished;
		accessMutex.Unlock();
		return result;
	}

	std::string getActivityText() {
		return progressString;
	}

	void startBackgroundThread() {
		//threadedMode = true;
		//curTextureProcessor = this;
		do{
			this->Event();
			Sleep(10);
		} while (!started);
	}

	void finish() {
		//threadedMode = false;
		//curTextureProcessor = NULL;
	}

	void init() {
		//dawn_debug_info("Starting initialization");
		Sleep(100);
		progressString = "Initializing Editor";
		//Editor.LoadTextures();
		Sleep(100);
		setProgress(0.025f);
		progressString = "Initializing GUI";
		//GUI.LoadTextures();
		//GUI.SetPlayer(Globals::getPlayer());
		Sleep(100);
		setProgress(0.05f);
		progressString = "Initializing Character Screen";
		//characterInfoScreen = std::auto_ptr<CharacterInfoScreen>(new CharacterInfoScreen(Globals::getPlayer()));
		//characterInfoScreen->LoadTextures();
		Sleep(100);
		setProgress(0.075f);
		progressString = "Initializing Inventory Screen";
		//inventoryScreen = std::auto_ptr<InventoryScreen>(new InventoryScreen(Globals::getPlayer()));
		//inventoryScreen->loadTextures();
		Sleep(100);
		setProgress(0.1f);
		progressString = "Initializing Action Bar";
		//actionBar = std::auto_ptr<ActionBar>(new ActionBar(Globals::getPlayer()));
		//actionBar->loadTextures();
		Sleep(100);
		setProgress(0.125f);
		progressString = "Initializing Spellbook";
		//spellbook = std::auto_ptr<Spellbook>(new Spellbook(Globals::getPlayer()));
		//spellbook->loadTextures();
		Sleep(100);
		setProgress(0.15f);
		progressString = "Initializing Log Window";
		//logWindow = std::auto_ptr<LogWindow>(new LogWindow);
		//logWindow->loadTextures();
		Sleep(100);
		setProgress(0.16f);
		progressString = "Initializing Buff Display";
		//buffWindow = std::auto_ptr<BuffWindow>(new BuffWindow(Globals::getPlayer()));
		Sleep(100);
		setProgress(0.175f);
		progressString = "Initializing Quest Screen";
		//questWindow = std::auto_ptr<QuestWindow>(new QuestWindow);
		Sleep(100);
		setProgress(0.2f);
		progressString = "Initializing Menu Screen";
		//optionsWindow = std::auto_ptr<OptionsWindow>(new OptionsWindow);

		Sleep(100);
		setProgress(0.3f);
		Sleep(100);
		setProgress(0.4f);
		Sleep(100);
		setProgress(0.5f);
		Sleep(100);
		setProgress(0.6f);
		Sleep(100);
		setProgress(0.8f);
		Sleep(100);
		setProgress(0.9f);
		Sleep(100);
		setProgress(1.0f);
		Sleep(100);

		accessMutex.Lock();
		finished = true;
		accessMutex.Unlock();
	}

	static LoadingManager& Get() {
		static LoadingManager instance;
		return instance;
	}

private:

	LoadingManager() : finished(false), progressString("-"), progress(0.0), accessMutex(), started(false) {
		SetThreadType(ThreadTypeEventDriven);
	}

	BOOL OnTask() override {
		if (started) {
			return true;
		}

		started = true;
		init();
		return true;
	}

	bool started;
	bool finished;
	std::string progressString;
	float progress;
	CMutexClass accessMutex;
};