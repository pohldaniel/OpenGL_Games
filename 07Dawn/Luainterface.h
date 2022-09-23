#pragma once

#include "Luafunctions.h"

namespace TileClassificationType {
	enum TileClassificationType;
}

class TileSet;
class CCharacter;

namespace EditorInterface{
	TileSet *getTileSet();
	TileSet *getTileSet(TileClassificationType::TileClassificationType tileType);
	void addGroundTile(int posX, int posY, int tile);
	void addEnvironment(int posX, int posY, int posZ, int tile);
	void adjustLastRGBA(double red, double green, double blue, double alpha);
	//void adjustLastScale(double scaleX, double scaleY);
	void addCollisionRect(int lrx, int lry, int width, int height);
}

namespace DawnInterface{
	void enterZone(std::string zoneName, int enterX, int enterY);
	void setCurrentZone(std::string zoneName);

	CCharacter* createNewMobType(std::string typeID);
}