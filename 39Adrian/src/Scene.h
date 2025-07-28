#pragma once
#include <engine/MeshObject/Shape.h>
#include <engine/Md2Model.h>
#include <engine/CharacterSet.h>
#include <engine/Texture.h>

struct Scene {

	friend class SceneManager;

public:

	Scene();
	~Scene();
	void loadScene();
	void loadBuilding(const char* filename, bool changeWinding = false);
	void loadMiniMap(const char* filename);
	void loadFont();
	void loadBackground(const std::string& background);

	Md2Model heroModel, hueteotl, mutantman, corpse, mutantlizard, mutantcheetah, ripper;
	std::vector<Shape> buildings;
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
};