#pragma once

class Game;

class IngameUI {
public:
	static bool visible;

	static int state;

	static bool setup;

	static void Setup();

	static void Draw(Game* game);

	static void DrawIngame(Game* game);

	static void DrawOptions(Game* game);

};