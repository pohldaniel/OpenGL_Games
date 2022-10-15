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

class CSpellActionBase;
class GeneralRayDamageSpell;
class GeneralAreaDamageSpell;
class GeneralBoltDamageSpell;
class GeneralHealingSpell;
class GeneralBuffSpell;
class MeleeDamageAction;
class RangedDamageAction;

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
	void addMobSpawnPoint(std::string mobID, std::string name, int x_pos, int y_pos, int respawn_rate, int do_respawn, Enums::Attitude attitude);
	
	void removeInteractionRegion(InteractionRegion *regionToRemove);	
	void removeInteractionPoint(InteractionPoint *pointToRemove);
	std::string getItemReferenceRestore(Character *character);
	TextWindow *createTextWindow();

	GeneralRayDamageSpell* createGeneralRayDamageSpell();
	GeneralAreaDamageSpell* createGeneralAreaDamageSpell();
	GeneralBoltDamageSpell* createGeneralBoltDamageSpell();
	GeneralHealingSpell* createGeneralHealingSpell();
	GeneralBuffSpell* createGeneralBuffSpell();
	MeleeDamageAction* createMeleeDamageAction();
	RangedDamageAction* createRangedDamageAction();
	GeneralRayDamageSpell* copySpell(GeneralRayDamageSpell *other);
	GeneralAreaDamageSpell* copySpell(GeneralAreaDamageSpell *other);
	GeneralBoltDamageSpell* copySpell(GeneralBoltDamageSpell *other);
	GeneralHealingSpell* copySpell(GeneralHealingSpell *other);
	GeneralBuffSpell* copySpell(GeneralBuffSpell *other);
	MeleeDamageAction* copySpell(MeleeDamageAction *other);
	RangedDamageAction* copySpell(RangedDamageAction *other);
	void inscribeSpellInPlayerSpellbook(CSpellActionBase *inscribedSpell);
}