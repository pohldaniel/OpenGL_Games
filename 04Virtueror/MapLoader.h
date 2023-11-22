#pragma once
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Constants.h"

class IsoMap;
class GameMap;

struct MapLoader {

public:
	MapLoader();
	MapLoader(GameMap * gm, IsoMap * im);

	void setMaps(GameMap * gm, IsoMap * im);
	bool loadLevel(const std::string & filename);

private:
	void readBaseData(std::fstream & fs);
	bool readObjectsData(std::fstream & fs);

	GameMap * mGameMap = nullptr;
	IsoMap * mIsoMap = nullptr;
};