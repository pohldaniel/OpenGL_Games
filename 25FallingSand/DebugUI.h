#pragma once

#include <vector>
#include <engine/Texture.h>

class Game;
class Material;

class DebugUI {
public:
	static bool visible;

	static void Draw(Game* game);
};

class DebugCheatsUI {
public:
	static bool visible;
	static std::vector<Texture*> images;

	static void Setup();

	static void Draw(Game* game);
};

class DebugDrawUI {
public:
	static bool visible;
	static int selIndex;
	static std::vector<Texture*> images;

	static Material* selectedMaterial;
	static unsigned char brushSize;

	static void Setup();

	static void Draw(Game* game);
};