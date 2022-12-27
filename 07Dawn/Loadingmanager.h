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
#include "OptionsWindow.h"
#include "Luainterface.h"
#include "Application.h"
#include "LoadingManager.h"


class LoadingManager : public CThread {

public:
	
	enum Entry {
		GAME = 0,
		EDITOR = 1,
		LOAD = 2,
	};

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

	void startBackgroundThread(Entry entry) {
		m_entry = entry;

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
		LuaFunctions::executeLuaFile("res/_lua/playerdata.lua");
		Player::Get().init();

		setProgress(0.025f);
		progressString = "Loading Spell Data";
		LuaFunctions::executeLuaFile("res/_lua/spells.lua");

		setProgress(0.05f);
		progressString = "Loading Item Data";
		LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");

		setProgress(0.075f);
		progressString = "Loading Mob Data";
		LuaFunctions::executeLuaFile("res/_lua/mobdata.lua");

		setProgress(0.1f);
		progressString = "Loading Interface Data";
		Game::Init();

		setProgress(0.125f);
		progressString = "Initializing Interface";
		Interface::Get().setPlayer(&Player::Get());
		Interface::Get().init({ Game::TextureRects.begin(), Game::TextureRects.begin() + 44 });
		Interface::Get().inscribeActionsToActionsbar();

		setProgress(0.15f);
		progressString = "Initializing Spellbook";
		Spellbook::Get().setPlayer(&Player::Get());
		Spellbook::Get().init({ Game::TextureRects.begin() + 44, Game::TextureRects.begin() + 48 });
		Spellbook::Get().reloadSpellsFromPlayer();

		setProgress(0.16f);
		progressString = "Initializing Character Info Canvas";
		CharacterInfo::Get().setPlayer(&Player::Get());
		CharacterInfo::Get().init({ Game::TextureRects.begin() + 48, Game::TextureRects.begin() + 53 });

		setProgress(0.175f);
		progressString = "Initializing Inventory Canvas";
		InventoryCanvas::Get().setPlayer(&Player::Get());
		InventoryCanvas::Get().init({ Game::TextureRects.begin() + 53, Game::TextureRects.begin() + 71 });

		setProgress(0.2f);
		progressString = "Initializing Quest Canvas";
		QuestCanvas::Get().init({ Game::TextureRects[71] });

		setProgress(0.3f);
		progressString = "Initializing Dialog Canvas";
		DialogCanvas::Init({ Game::TextureRects.begin() + 72, Game::TextureRects.begin() + 81 });

		setProgress(0.4f);
		progressString = "Initializing Shop Canvas";
		ShopCanvas::Get().init({ Game::TextureRects.begin() + 79, Game::TextureRects.begin() + 84 });

		setProgress(0.5f);
		progressString = "Initializing Shop Canvas";
		ShopCanvas::Get().init({ Game::TextureRects.begin() + 81, Game::TextureRects.begin() + 86 });

		setProgress(0.6f);
		progressString = "Initializing Interaction Points";
		InteractionPoint::Init({ Game::TextureRects.begin() + 86, Game::TextureRects.begin() + 92 });

		setProgress(0.8f);
		progressString = "Initializing GroundLoot";
		GroundLoot::Init({ Game::TextureRects.begin() + 92, Game::TextureRects.begin() + 95 });

		setProgress(0.9f);
		progressString = "Initializing Tooltips";
		ItemTooltip::Init({ Game::TextureRects.begin() + 55, Game::TextureRects.begin() + 58 });

		setProgress(1.0f);
		OptionsWindow::Get().init({ Game::TextureRects[95], Game::TextureRects[47], Game::TextureRects[46], Game::TextureRects[4] });
		progressString = "Initialize Game";

		LuaFunctions::executeLuaFile("res/_lua/loadsave.lua");
		LuaFunctions::executeLuaFile("res/_lua/gameinit.lua");

		DawnInterface::clearLogWindow();
		GLfloat color[] = { 1.0f, 1.0f, 0.0f };
		DawnInterface::addTextToLogWindow(color, "Welcome to the world of Dawn, %s.", Player::Get().getName().c_str());


		accessMutex.Lock();
		finished = true;
		Game::s_init = true;
		//DawnInterface::enterZone("res/_lua/arinoxGeneralShop", -158, 0);
		DawnInterface::enterZone("res/_lua/zone1", 512, 400);
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
		Player::Get().init();

		setProgress(0.025f);
		progressString = "Loading Spell Data";
		LuaFunctions::executeLuaFile("res/_lua/spells.lua");

		setProgress(0.05f);
		progressString = "Loading Item Data";
		LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");

		setProgress(0.075f);
		progressString = "Loading Mob Data";
		LuaFunctions::executeLuaFile("res/_lua/mobdata.lua");

		setProgress(1.0f);
		progressString = "Loading Interface Data";
		Editor::Init();

		accessMutex.Lock();
		finished = true;
		Editor::s_init = true;
		DawnInterface::enterZone("res/_lua/zone1", 512, 400);
		wglDeleteContext(Application::LoaderContext);
		accessMutex.Unlock();
	}

	void initLoad() {
		HDC hDC = GetDC(Application::Window);
		wglMakeCurrent(hDC, Application::LoaderContext);
		ReleaseDC(Application::Window, hDC);

		progressString = "Initializing Player";
		LuaFunctions::executeLuaFile("res/_lua/playerdata.lua");

		setProgress(0.025f);
		progressString = "Loading Spell Data";
		LuaFunctions::executeLuaFile("res/_lua/spells.lua");

		setProgress(0.05f);
		progressString = "Loading Item Data";
		LuaFunctions::executeLuaFile("res/_lua/itemdatabase.lua");

		setProgress(0.075f);
		progressString = "Loading Mob Data";
		LuaFunctions::executeLuaFile("res/_lua/mobdata.lua");

		setProgress(0.1f);
		progressString = "Loading Interface Data";
		Game::Init();

		setProgress(0.125f);
		progressString = "Initializing Interface";
		Interface::Get().setPlayer(&Player::Get());
		Interface::Get().init({ Game::TextureRects.begin(), Game::TextureRects.begin() + 44 });

		setProgress(0.15f);
		progressString = "Initializing Spellbook";
		Spellbook::Get().setPlayer(&Player::Get());
		Spellbook::Get().init({ Game::TextureRects.begin() + 44, Game::TextureRects.begin() + 48 });

		setProgress(0.16f);
		progressString = "Initializing Character Info Canvas";
		CharacterInfo::Get().setPlayer(&Player::Get());
		CharacterInfo::Get().init({ Game::TextureRects.begin() + 48, Game::TextureRects.begin() + 53 });

		setProgress(0.175f);
		progressString = "Initializing Inventory Canvas";
		InventoryCanvas::Get().setPlayer(&Player::Get());
		InventoryCanvas::Get().init({ Game::TextureRects.begin() + 53, Game::TextureRects.begin() + 71 });

		setProgress(0.2f);
		progressString = "Initializing Quest Canvas";
		QuestCanvas::Get().init({ Game::TextureRects[71] });

		setProgress(0.3f);
		progressString = "Initializing Dialog Canvas";
		DialogCanvas::Init({ Game::TextureRects.begin() + 72, Game::TextureRects.begin() + 81 });

		setProgress(0.4f);
		progressString = "Initializing Shop Canvas";
		ShopCanvas::Get().init({ Game::TextureRects.begin() + 81, Game::TextureRects.begin() + 86 });

		setProgress(0.5f);
		progressString = "Initializing Interaction Points";
		InteractionPoint::Init({ Game::TextureRects.begin() + 86, Game::TextureRects.begin() + 92 });

		setProgress(0.8f);
		progressString = "Initializing GroundLoot";
		GroundLoot::Init({ Game::TextureRects.begin() + 92, Game::TextureRects.begin() + 95 });

		setProgress(0.9f);
		progressString = "Initializing Tooltips";
		ItemTooltip::Init({ Game::TextureRects.begin() + 55, Game::TextureRects.begin() + 58 });

		setProgress(1.0f);
		OptionsWindow::Get().init({ Game::TextureRects[95], Game::TextureRects[47], Game::TextureRects[46], Game::TextureRects[4] });
		progressString = "Initialize Game";

		LuaFunctions::executeLuaFile("res/_lua/loadsave.lua");

		accessMutex.Lock();
		finished = true;
		Game::s_init = true;
		LuaFunctions::executeLuaScript("loadGame( 'res/_lua/save/savegame' )");
		DawnInterface::clearLogWindow();
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

		switch (m_entry) {
			case Entry::GAME:
				init();
				break;
			case Entry::EDITOR:
				initEditor();
				break;
			case Entry::LOAD:
				initLoad();
				break;
			default:
				break;
		}
		return true;
	}

	bool started;
	bool finished;
	std::string progressString;
	float progress;
	CMutexClass accessMutex;
	Entry m_entry;
};