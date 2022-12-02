#pragma once
#include <cstdarg>
#include "Luafunctions.h"

namespace Enums {
	enum TileClassificationType;
	enum Attitude;
	enum ItemQuality;
	enum ItemType;
	enum ArmorType;
	enum WeaponType;
}

namespace EquipPosition {
	enum EquipPosition;
}

struct TextureRect;
class TileSet;
class Character;
class Npc;
struct CharacterType;
class InteractionRegion;
class InteractionPoint;
class TextWindow;

class SpellActionBase;
class GeneralRayDamageSpell;
class GeneralAreaDamageSpell;
class GeneralBoltDamageSpell;
class GeneralHealingSpell;
class GeneralBuffSpell;
class MeleeDamageAction;
class RangedDamageAction;
class Player;
class Shop;
class Zone;
class Item;
class Quest;
class LuaCallIndirection;

namespace EditorInterface{
	TileSet& getTileSet(Enums::TileClassificationType tileType);
	void addGroundTile(int posX, int posY, int tile);
	void addEnvironment(int posX, int posY, int posZ, int tile);
	void adjustLastRGBA(float red, float green, float blue, float alpha);
	void adjustLastSize(float width, float height);
	void addCollisionRect(int lrx, int lry, int width, int height);
}

namespace DawnInterface{
	TextureRect& loadimage(std::string file);

	const CharacterType& createNewMobType(std::string typeID);
	const InteractionRegion& addInteractionRegion();
	const InteractionPoint& addInteractionPoint();
	const InteractionPoint& addCharacterInteractionPoint(Character *character);

	void enterZone(std::string zoneName, int enterX, int enterY);
	void setCurrentZone(std::string zoneName);	
	Npc* addMobSpawnPoint(std::string mobID, std::string name, int x_pos, int y_pos, int respawn_rate, int do_respawn, Enums::Attitude attitude);
	
	void removeInteractionRegion(InteractionRegion *regionToRemove);	
	void removeInteractionPoint(InteractionPoint *pointToRemove);
	std::string getItemReferenceRestore(Character *character);
	TextWindow* createTextWindow();

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
	void inscribeSpellInPlayerSpellbook(SpellActionBase *inscribedSpell);
	void addTextToLogWindow(float color[], const char *text, ...);
	void clearLogWindow();
	std::string getInventorySaveText();
	void restoreItemInBackpack(Item* item, int inventoryPosX, int inventoryPosY, size_t stackSize);
	void restoreWieldItem(int slot, Item* item);
	void giveItemToPlayer(Item* item);
	Item* createNewItem(std::string name, int sizeX, int sizeY, std::string symbolFile, Enums::ItemQuality itemQuality, EquipPosition::EquipPosition equipPos, Enums::ItemType itemType, Enums::ArmorType armorType, Enums::WeaponType weaponType);
	Player& getPlayer();
	const Zone& getCurrentZone();
	const Shop& addShop(std::string name);
	Quest* addQuest(std::string questName, std::string questDescription);
	LuaCallIndirection* createEventHandler();
}