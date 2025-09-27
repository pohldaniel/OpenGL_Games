#pragma once

#include <experimental/filesystem>
#include <vector>
#include <fstream>
#include <tuple>
#include <regex>

#include <engine/Texture.h>

#include "Utime.h"
#include "World.h"

class Game;

class MainMenuUI {
public:
	static bool visible;

	static int state;

	static bool setup;

	//static GPU_Image* title;
	static Texture* title;

	static bool connectButtonEnabled;

	static ImVec2 pos;

	static std::vector<std::tuple<std::string, WorldMeta>> worlds;

	static long long lastRefresh;

	static void RefreshWorlds(Game* game);

	static void Setup();

	static void Draw(Game* game);

	static void DrawMainMenu(Game* game);

	static void DrawSingleplayer(Game* game);
	static void DrawMultiplayer(Game* game);

	static void DrawCreateWorld(Game* game);

	static void DrawOptions(Game* game);

};