#pragma once

#include <array>
#include <functional>
#include <unordered_set>
#include <unordered_map>

#include <engine/MeshObject/Shape.h>
#include <engine/Md2Model.h>
#include <engine/CharacterSet.h>
#include <engine/Texture.h>

struct BotInfo {
	int type;
	std::array<float, 2> start;
	std::array<float, 2> end;
	float speed;
	float angle;
};

struct Scene {

	friend class SceneManager;
	friend class Adrian;
	friend class MiniMap;

public:

	Scene(const std::string& background);
	~Scene();
	void loadScene();
	void loadBuilding(const char* filename, bool changeWinding = false);
	void loadMiniMap(const char* filename);
	void loadBots(const char* filename);
	void loadFont();

private:

	Md2Model heroModel, hueteotl, mutantman, corpse, mutantlizard, mutantcheetah, ripper;
	std::vector<Shape> buildings;
	std::vector<BotInfo> bots;
	Shape segment, disk, sphere;
	std::vector<std::array<float, 4>> buildingsMiniMap;

	float xconvfactor;
	float yconvfactor;
	CharacterSet characterSet;

	std::tuple<std::string, std::string> labels[7] = {
		{"RIGHT CLICK ON ENEMYS TORSO", "TO KILL IT"},
		{"THE CORPSE", "SLOWER THAN HERO"},
		{"MUTANT CHEETA", "ITS FAST ENOUGH TO BE KILLED"},
		{"MUTANT LIZARD", "SLOWER THAN MUTANT MAN"},
		{"MUTANT MAN", "AS FAST AS HERO"},
		{"RIPPER", "VERY FAST TRY SNEAKING PAST IT"},
		{"THE UNDEAD", "VERY SLOW MUTANT"}
	};

	Texture texture1, texture2, background;
	std::vector<TextureRect> tileSet;
	unsigned int atlas;

	std::unordered_set<std::array<int, 2>, std::function<size_t(const std::array<int, 2>&)>, std::function<bool(const std::array<int, 2>&, const std::array<int, 2>&)>> addedTiles;
};