#pragma once
#include <map>
#include <algorithm>
#include "Settings.h"

class Game;

class OptionsUI {

	//static std::map<std::string, FMOD::Studio::Bus*> busMap;

public:
	static int item_current_idx;
	static bool vsync;
	static bool minimizeOnFocus;

	static void Draw(Game* game);
	static void DrawGeneral(Game* game);
	static void DrawVideo(Game* game);
	static void DrawAudio(Game* game);
	static void DrawInput(Game* game);
};