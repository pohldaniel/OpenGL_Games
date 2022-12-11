#pragma once

#include <string>
#include "thread/Thread.h"
#include "Game.h"
#include "Editor.h"
#include "Player.h"
#include "Interface.h"
#include "Spellbook.h"
#include "InventoryCanvas.h"
#include "Shop.h"
#include "Quest.h"
#include "InteractionPoint.h"
#include "Groundloot.h"
#include "Luainterface.h"
#include "Application.h"

class LoadingManager : public CThread {

public:
	void setProgress(float newProgress) {
		progress = newProgress;
		Sleep(1);
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

	void startBackgroundThread(bool initEditor) {
		m_initEditor = initEditor;
		do {
			
			this->Event();
			Sleep(10);
		} while (!started);
	}

	void init() {
		HDC hDC = GetDC(Application::Window);
		wglMakeCurrent(hDC, Application::LoaderContext);
		ReleaseDC(Application::Window, hDC);

		progressString = "Initializing Player";
		LuaFunctions::executeLuaFile("res/_lua/playerdata_w.lua");
		Player::Get().setCharacterType("player_w");
		Player::Get().setClass(Enums::CharacterClass::Liche);

		setProgress(0.025f);
		progressString = "Loading Spell Data";
		LuaFunctions::executeLuaFile("res/_lua/spells.lua");

		setProgress(0.05f);
		progressString = "Loading Item Data";
		LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");

		setProgress(0.075f);
		progressString = "Loading Mob Data";
		LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");

		setProgress(0.1f);
		progressString = "Loading Interface Data";
		Game::Init();

		setProgress(0.125f);
		progressString = "Initializing Interface";
		Interface::Get().setPlayer(&Player::Get());
		Interface::Get().init({ Game::TextureRects.begin(), Game::TextureRects.begin() + 42 });

		setProgress(0.15f);
		progressString = "Initializing Spellbook";
		Spellbook::Get().setPlayer(&Player::Get());
		Spellbook::Get().init({ Game::TextureRects.begin() + 42, Game::TextureRects.begin() + 46 });
		Spellbook::Get().reloadSpellsFromPlayer();

		setProgress(0.16f);
		progressString = "Initializing Character Info Canvas";
		CharacterInfo::Get().setPlayer(&Player::Get());
		CharacterInfo::Get().init({ Game::TextureRects.begin() + 46, Game::TextureRects.begin() + 51 });

		setProgress(0.175f);
		progressString = "Initializing Inventory Canvas";
		InventoryCanvas::Get().setPlayer(&Player::Get());
		InventoryCanvas::Get().init({ Game::TextureRects.begin() + 51, Game::TextureRects.begin() + 69 });

		setProgress(0.2f);
		progressString = "Initializing Quest Canvas";
		QuestCanvas::Get().init({ Game::TextureRects[69] });

		setProgress(0.3f);
		progressString = "Initializing Dialog Canvas";
		DialogCanvas::Init({ Game::TextureRects.begin() + 70, Game::TextureRects.begin() + 79 });

		setProgress(0.4f);
		progressString = "Initializing Shop Canvas";
		ShopCanvas::Get().init({ Game::TextureRects.begin() + 79, Game::TextureRects.begin() + 84 });

		setProgress(0.5f);
		progressString = "Initializing Shop Canvas";
		ShopCanvas::Get().init({ Game::TextureRects.begin() + 79, Game::TextureRects.begin() + 84 });

		setProgress(0.6f);
		progressString = "Initializing Interaction Points";
		InteractionPoint::Init({ Game::TextureRects.begin() + 84, Game::TextureRects.begin() + 90 });

		setProgress(0.8f);
		progressString = "Initializing GroundLoot";
		GroundLoot::Init({ Game::TextureRects.begin() + 90, Game::TextureRects.begin() + 93 });

		setProgress(0.9f);
		progressString = "Initializing Tooltips";
		ItemTooltip::Init({ Game::TextureRects.begin() + 53, Game::TextureRects.begin() + 56 });

		setProgress(1.0f);
		progressString = "Initialize Game";
		LuaFunctions::executeLuaFile("res/_lua/gameinit.lua");
		DawnInterface::clearLogWindow();
		GLfloat color[] = { 1.0f, 1.0f, 0.0f };
		DawnInterface::addTextToLogWindow(color, "Welcome to the world of Dawn, %s.", Player::Get().getName().c_str());
		DawnInterface::enterZone("res/_lua/zone1", 512, 400);

		accessMutex.Lock();
		finished = true;
		Game::s_init = true;
		wglDeleteContext(Application::LoaderContext);
		accessMutex.Unlock();
	}

	void initEditor() {
		HDC hDC = GetDC(Application::Window);
		wglMakeCurrent(hDC, Application::LoaderContext);
		ReleaseDC(Application::Window, hDC);

		progressString = "Initializing Player";
		LuaFunctions::executeLuaFile("res/_lua/playerdata_w.lua");
		Player::Get().setCharacterType("player_w");
		Player::Get().setClass(Enums::CharacterClass::Liche);

		setProgress(0.025f);
		progressString = "Loading Spell Data";
		LuaFunctions::executeLuaFile("res/_lua/spells.lua");

		setProgress(0.05f);
		progressString = "Loading Item Data";
		LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");

		setProgress(0.075f);
		progressString = "Loading Mob Data";
		LuaFunctions::executeLuaFile("res/_lua/mobdata_wolf.lua");

		setProgress(1.0f);
		progressString = "Loading Interface Data";
		Editor::Init();

		accessMutex.Lock();
		finished = true;
		Editor::s_init = true;
		wglDeleteContext(Application::LoaderContext);
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
		m_initEditor ? initEditor() : init();
		return true;
	}

	bool started;
	bool finished;
	std::string progressString;
	float progress;
	CMutexClass accessMutex;
	bool m_initEditor;
};