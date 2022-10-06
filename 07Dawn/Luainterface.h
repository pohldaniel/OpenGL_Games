#pragma once

#include "Luafunctions.h"

namespace Enums {
	enum TileClassificationType;
}

namespace Enums {
	enum Attitude;
}

class TileSet;
class Character;
class Npc;
struct CharacterType;
class InteractionRegion;
class InteractionPoint;
class TextWindow;

namespace EditorInterface{
	TileSet& getTileSet(Enums::TileClassificationType tileType);
	void addGroundTile(int posX, int posY, int tile);
	void addEnvironment(int posX, int posY, int posZ, int tile);
	void adjustLastRGBA(float red, float green, float blue, float alpha);
	void adjustLastSize(float width, float height);
	void addCollisionRect(int lrx, int lry, int width, int height);
}

namespace DawnInterface{
	const CharacterType& createNewMobType(std::string typeID);
	const InteractionRegion& addInteractionRegion();
	const InteractionPoint& addInteractionPoint();
	const InteractionPoint& addCharacterInteractionPoint(Character *character);

	void enterZone(std::string zoneName, int enterX, int enterY);
	void setCurrentZone(std::string zoneName);	
	void addMobSpawnPoint(std::string mobID, int x_pos, int y_pos, int respawn_rate, int do_respawn, Enums::Attitude attitude);
	
	void removeInteractionRegion(InteractionRegion *regionToRemove);	
	void removeInteractionPoint(InteractionPoint *pointToRemove);
	std::string getItemReferenceRestore(Character *character);
	TextWindow *createTextWindow();
}