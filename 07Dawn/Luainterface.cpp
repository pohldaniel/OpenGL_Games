#include "Luainterface.h"

#include "TilesetManager.h"
#include "Zone.h"

#include "Constants.h"
#include "Npc.h"
#include "InteractionRegion.h"
#include "InteractionPoint.h"
#include "TextWindow.h"

namespace EditorInterface{
	
	void addGroundTile(int posX, int posY, int tile){		
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		const TileSet& tileSet = EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR);
		currentZone->getTileMap().push_back({ posX, posY, tileSet.getTile(tile) });
	}

	void addEnvironment(int posX, int posY, int posZ, int tile){
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		const TileSet& tileSet = EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT);
		currentZone->getEnvironmentMap().push_back({ posX, posY, posZ, tileSet.getTile(tile), 1.0f, 1.0f, 1.0f, 1.0f, static_cast<float>(tileSet.getTile(tile).textureRect.width), static_cast<float>(tileSet.getTile(tile).textureRect.height) });
	}

	void adjustLastRGBA(float red, float green, float blue, float alpha){
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		EnvironmentMap &lastEnv = currentZone->getEnvironmentMap()[currentZone->getEnvironmentMap().size() - 1];
		lastEnv.red = red;
		lastEnv.green = green;
		lastEnv.blue = blue;
		lastEnv.transparency = alpha;
	}

	void adjustLastSize(float width, float height){
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		EnvironmentMap &lastEnv = currentZone->getEnvironmentMap()[currentZone->getEnvironmentMap().size() - 1];
		lastEnv.width = width;
		lastEnv.height = height;
	}

	void addCollisionRect(int lrx, int lry, int width, int height){
		Zone *currentZone = ZoneManager::Get().getCurrentZone();
		currentZone->getCollisionMap().push_back({ lrx, lry, width, height });
	}

	TileSet& getTileSet(Enums::TileClassificationType tileType) {
		return TileSetManager::Get().getTileSet(tileType);
	}
}
namespace DawnInterface{

	void enterZone(std::string zoneName, int enterX, int enterY){
		Zone& newZone = ZoneManager::Get().getZone(zoneName);
		if (!newZone.zoneDataLoaded()) {
			newZone.loadZone();
		}
		ZoneManager::Get().setCurrentZone(&newZone);
		
		//getPlayer()->setPosition(enterX, enterY);
		std::ostringstream oss;
		std::string zoneNameNoPrefix = zoneName;
		if (zoneNameNoPrefix.find_last_of('/') != std::string::npos){
			zoneNameNoPrefix = zoneNameNoPrefix.substr(zoneNameNoPrefix.find_last_of('/') + 1);
		}
		oss << "if (" << zoneNameNoPrefix << ".onEnterMap ~= nil)\nthen\n    " << zoneNameNoPrefix << ".onEnterMap(" << enterX << "," << enterY << ");\nelse    print \"" << zoneNameNoPrefix << ".onEnterMap was not defined\";\nend";
		std::string onEnterCall = oss.str();
		LuaFunctions::executeLuaScript(onEnterCall);
	}

	void setCurrentZone(std::string zoneName){

		Zone& newCurZone = ZoneManager::Get().getZone(zoneName);
		ZoneManager::Get().setCurrentZone(&newCurZone);
	}

	const CharacterType& createNewMobType(std::string characterType) {
		return CharacterTypeManager::Get().getCharacterType(characterType);
	}

	void addMobSpawnPoint(std::string mobID, std::string name, int x_pos, int y_pos, int respawn_rate, int do_respawn, Enums::Attitude attitude) {
		
		if (!CharacterTypeManager::Get().containsCaracterType(mobID)) {
			return;
		}

		Npc* newMob = new Npc(CharacterTypeManager::Get().getCharacterType(mobID), 0, 0, 0, 0, 0);	
		newMob->baseOnType(mobID);
		newMob->setSpawnInfo(x_pos, y_pos, respawn_rate, do_respawn);
		newMob->setAttitude(attitude);
		newMob->setName(name);
		//newMob->setActiveGUI(&GUI);
		ZoneManager::Get().getCurrentZone()->addNPC(newMob);
	}

	const InteractionRegion& addInteractionRegion(){
		ZoneManager::Get().getCurrentZone()->addInteractionRegion(InteractionRegion());
		return ZoneManager::Get().getCurrentZone()->getInteractionRegions().back();
	}

	void removeInteractionRegion(InteractionRegion *regionToRemove){
		regionToRemove->markAsDeletable();
	}

	const InteractionPoint& addInteractionPoint(){
		ZoneManager::Get().getCurrentZone()->addInteractionPoint(InteractionPoint());
		return ZoneManager::Get().getCurrentZone()->getInteractionPoints().back();
	}

	const InteractionPoint& addCharacterInteractionPoint(Character *character){
		ZoneManager::Get().getCurrentZone()->addInteractionPoint(CharacterInteractionPoint(character));
		return ZoneManager::Get().getCurrentZone()->getInteractionPoints().back();
	}

	void removeInteractionPoint(InteractionPoint *pointToRemove){
		//pointToRemove->markAsDeletable();
	}

	std::string getItemReferenceRestore(Character *character){
		if (character == NULL) {
			return "nil;";
		}
		for (std::unordered_map< std::string, Zone >::iterator it = ZoneManager::Get().getAllZones().begin(); it != ZoneManager::Get().getAllZones().end(); ++it) {
			Zone curZone = it->second;
			bool found;
			size_t foundPos;
			curZone.findCharacter(character, found, foundPos);
			if (found) {
				std::ostringstream oss;
				oss << "DawnInterface.restoreCharacterReference( \"" << curZone.getZoneName() << "\", " << foundPos << " )";
				return oss.str();
			}
		}
		return "";
	}

	TextWindow *createTextWindow(){
		TextWindow *newTextWindow = new TextWindow();
		Globals::allTextWindows.push_back(newTextWindow);
		newTextWindow->toggle();
		return newTextWindow;
	}

	GeneralRayDamageSpell* createGeneralRayDamageSpell() {
		std::auto_ptr<GeneralRayDamageSpell> newSpell(dynamic_cast<GeneralRayDamageSpell*>(SpellCreation::getGeneralRayDamageSpell()));
		return newSpell.release();
	}

	GeneralAreaDamageSpell* createGeneralAreaDamageSpell() {
		std::auto_ptr<GeneralAreaDamageSpell> newSpell(dynamic_cast<GeneralAreaDamageSpell*>(SpellCreation::getGeneralAreaDamageSpell()));
		return newSpell.release();
	}

	GeneralBoltDamageSpell* createGeneralBoltDamageSpell() {
		std::auto_ptr<GeneralBoltDamageSpell> newSpell(dynamic_cast<GeneralBoltDamageSpell*>(SpellCreation::getGeneralBoltDamageSpell()));
		return newSpell.release();
	}

	GeneralHealingSpell* createGeneralHealingSpell() {
		std::auto_ptr<GeneralHealingSpell> newSpell(dynamic_cast<GeneralHealingSpell*>(SpellCreation::getGeneralHealingSpell()));
		return newSpell.release();
	}

	GeneralBuffSpell* createGeneralBuffSpell() {
		std::auto_ptr<GeneralBuffSpell> newSpell(dynamic_cast<GeneralBuffSpell*>(SpellCreation::getGeneralBuffSpell()));
		return newSpell.release();
	}

	MeleeDamageAction* createMeleeDamageAction() {
		std::auto_ptr<MeleeDamageAction> newAction(dynamic_cast<MeleeDamageAction*>(SpellCreation::getMeleeDamageAction()));
		return newAction.release();
	}

	RangedDamageAction* createRangedDamageAction() {
		std::auto_ptr<RangedDamageAction> newAction(dynamic_cast<RangedDamageAction*>(SpellCreation::getRangedDamageAction()));
		return newAction.release();
	}

	GeneralRayDamageSpell* copySpell(GeneralRayDamageSpell *other) {
		std::auto_ptr<GeneralRayDamageSpell> newSpell(dynamic_cast<GeneralRayDamageSpell*>(SpellCreation::getGeneralRayDamageSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	GeneralAreaDamageSpell* copySpell(GeneralAreaDamageSpell *other) {
		std::auto_ptr<GeneralAreaDamageSpell> newSpell(dynamic_cast<GeneralAreaDamageSpell*>(SpellCreation::getGeneralAreaDamageSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	GeneralBoltDamageSpell* copySpell(GeneralBoltDamageSpell *other) {
		std::auto_ptr<GeneralBoltDamageSpell> newSpell(dynamic_cast<GeneralBoltDamageSpell*>(SpellCreation::getGeneralBoltDamageSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	GeneralHealingSpell* copySpell(GeneralHealingSpell *other) {
		std::auto_ptr<GeneralHealingSpell> newSpell(dynamic_cast<GeneralHealingSpell*>(SpellCreation::getGeneralHealingSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	GeneralBuffSpell* copySpell(GeneralBuffSpell *other) {
		std::auto_ptr<GeneralBuffSpell> newSpell(dynamic_cast<GeneralBuffSpell*>(SpellCreation::getGeneralBuffSpell(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	MeleeDamageAction* copySpell(MeleeDamageAction *other) {
		std::auto_ptr<MeleeDamageAction> newSpell(dynamic_cast<MeleeDamageAction*>(SpellCreation::getMeleeDamageAction(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}

	RangedDamageAction* copySpell(RangedDamageAction *other) {
		std::auto_ptr<RangedDamageAction> newSpell(dynamic_cast<RangedDamageAction*>(SpellCreation::getRangedDamageAction(other)));
		newSpell->unsetLuaID();
		return newSpell.release();
	}
}