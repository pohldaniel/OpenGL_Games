#pragma once

#include <regex>
#include <engine/Texture.h>

class Game;

class CreateWorldUI {
public:
	static bool setup;
	static char worldNameBuf[32];

	static Texture* materialTestWorld;
	static Texture* defaultWorld;

	static bool createWorldButtonEnabled;

	static std::string worldFolderLabel;

	static int selIndex;

	static void Setup();
	static void Reset(Game* game);

	static void Draw(Game* game);

	static void inputChanged(std::string text, Game* game);
};