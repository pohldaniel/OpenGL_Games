#pragma once

#include "Luafunctions.h"

namespace TileClassificationType {
	enum TileClassificationType;
}

namespace Attitude {
	enum Attitude;
}

class TileSet;
class Character;
class Npc;
struct CharacterType;

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
	const CharacterType& createNewMobType(std::string typeID);
	void addMobSpawnPoint(std::string mobID, int x_pos, int y_pos, int respawn_rate, int do_respawn, Attitude::Attitude attitude);
}