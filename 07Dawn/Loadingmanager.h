#pragma once

#include "thread/Thread.h"
#include "random.h"

#include <string>

class LoadingManager : public CThread {
public:

	bool started;
	bool finished;
	std::string progressString;
	double progress;
	CMutexClass accessMutex;
	LoadingManager() : finished(false), progressString(""), progress(0.0), accessMutex(), started(false) {
		SetThreadType(ThreadTypeEventDriven);
	}

	void setProgress(double newProgress) {
		progress = newProgress;
	}

	virtual double getProgress() {
		return progress;
	}

	virtual bool isFinished() {
		bool result = false;
		accessMutex.Lock();
		result = finished;
		accessMutex.Unlock();
		return result;
	}

	virtual std::string getActivityText() {
		return progressString;
	}

	virtual void startBackgroundThread() {
		//threadedMode = true;
		//curTextureProcessor = this;
		do{
			this->Event();
			Sleep(10);
		} while (!started);
	}

	virtual void finish() {
		//threadedMode = false;
		//curTextureProcessor = NULL;
	}

	void init() {
		//dawn_debug_info("Starting initialization");
		Sleep(100);
		progressString = "Initializing Editor";
		//Editor.LoadTextures();
		Sleep(100);
		setProgress(0.025);
		progressString = "Initializing GUI";
		//GUI.LoadTextures();
		//GUI.SetPlayer(Globals::getPlayer());
		Sleep(100);
		setProgress(0.05);
		progressString = "Initializing Character Screen";
		//characterInfoScreen = std::auto_ptr<CharacterInfoScreen>(new CharacterInfoScreen(Globals::getPlayer()));
		//characterInfoScreen->LoadTextures();
		Sleep(100);
		setProgress(0.075);
		progressString = "Initializing Inventory Screen";
		//inventoryScreen = std::auto_ptr<InventoryScreen>(new InventoryScreen(Globals::getPlayer()));
		//inventoryScreen->loadTextures();
		Sleep(100);
		setProgress(0.1);
		progressString = "Initializing Action Bar";
		//actionBar = std::auto_ptr<ActionBar>(new ActionBar(Globals::getPlayer()));
		//actionBar->loadTextures();
		Sleep(100);
		setProgress(0.125);
		progressString = "Initializing Spellbook";
		//spellbook = std::auto_ptr<Spellbook>(new Spellbook(Globals::getPlayer()));
		//spellbook->loadTextures();
		Sleep(100);
		setProgress(0.15);
		progressString = "Initializing Log Window";
		//logWindow = std::auto_ptr<LogWindow>(new LogWindow);
		//logWindow->loadTextures();
		Sleep(100);
		setProgress(0.16);
		progressString = "Initializing Buff Display";
		//buffWindow = std::auto_ptr<BuffWindow>(new BuffWindow(Globals::getPlayer()));
		Sleep(100);
		setProgress(0.175);
		progressString = "Initializing Quest Screen";
		//questWindow = std::auto_ptr<QuestWindow>(new QuestWindow);
		Sleep(100);
		setProgress(0.2);
		progressString = "Initializing Menu Screen";
		//optionsWindow = std::auto_ptr<OptionsWindow>(new OptionsWindow);

		accessMutex.Lock();
		finished = true;
		accessMutex.Unlock();
	}

	virtual BOOL OnTask() {
		if (started) {
			return true;
		}

		started = true;
		init();
		return true;
	}

};
