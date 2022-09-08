#pragma once

#include "Luafunctions.h"

#include "Constants.h"
#include "Tileset.h"

namespace EditorInterface{
	TileSet *getTileSet();
	void addGroundTile(int posX, int posY, int tile);
	void addEnvironment(int posX, int posY, int posZ, int tile);
	void adjustLastRGBA(double red, double green, double blue, double alpha);
	//void adjustLastScale(double scaleX, double scaleY);
	void addCollisionRect(int lrx, int lry, int width, int height);
}

namespace DawnInterface{
	void enterZone(std::string zoneName, int enterX, int enterY);
	void setCurrentZone(std::string zoneName);
}