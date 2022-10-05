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
		Zone *currentZone = Globals::getCurrentZone();
		const TileSet& tileSet = EditorInterface::getTileSet(Enums::TileClassificationType::FLOOR);
		currentZone->tileMap.push_back(TileMap(posX, posY, tileSet.getTile(tile)));
	}

	void addEnvironment(int posX, int posY, int posZ, int tile){
		Zone *currentZone = Globals::getCurrentZone();
		const TileSet& tileSet = EditorInterface::getTileSet(Enums::TileClassificationType::ENVIRONMENT);
		currentZone->environmentMap.push_back(EnvironmentMap(posX, posY, tileSet.getTile(tile), 1.0f, 1.0f, 1.0f, 1.0f, static_cast<float>(tileSet.getTile(tile).textureRect.width), static_cast<float>(tileSet.getTile(tile).textureRect.height), posZ));
	}

	void adjustLastRGBA(float red, float green, float blue, float alpha){
		Zone *currentZone = Globals::getCurrentZone();
		EnvironmentMap &lastEnv = currentZone->environmentMap[currentZone->environmentMap.size() - 1];
		lastEnv.red = red;
		lastEnv.green = green;
		lastEnv.blue = blue;
		lastEnv.transparency = alpha;
	}

	void adjustLastSize(float width, float height){
		Zone *currentZone = Globals::getCurrentZone();
		EnvironmentMap &lastEnv = currentZone->environmentMap[currentZone->environmentMap.size() - 1];
		lastEnv.width = width;
		lastEnv.height = height;
	}

	void addCollisionRect(int lrx, int lry, int width, int height){
		Zone *currentZone = Globals::getCurrentZone();
		currentZone->collisionMap.push_back({ lrx, lry, width, height });
	}

	TileSet& getTileSet(Enums::TileClassificationType tileType) {
		return TileSetManager::Get().getTileSet(tileType);
	}
}
namespace DawnInterface{

	void enterZone(std::string zoneName, int enterX, int enterY){
		Zone *newZone = Globals::allZones[zoneName];
		if (newZone == NULL) {
			newZone = new Zone();
			Globals::allZones[zoneName] = newZone;
		}
		if (!newZone->zoneDataLoaded()) {
			newZone->loadZone(zoneName);
		}
		Globals::setCurrentZone(newZone);
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

		if (Globals::allZones[zoneName] == NULL){
			Globals::allZones[zoneName] = new Zone();
		}
		Zone* newCurZone = Globals::allZones[zoneName];
		Globals::setCurrentZone(newCurZone);
	}

	const CharacterType& createNewMobType(std::string characterType) {
		return CharacterTypeManager::Get().getCharacterType(characterType);
	}

	void addMobSpawnPoint(std::string mobID, int x_pos, int y_pos, int respawn_rate, int do_respawn, Enums::Attitude attitude) {
	
		if (!CharacterTypeManager::Get().containsCaracterType(mobID)) {
			return;
		}
		Npc* newMob = new Npc(CharacterTypeManager::Get().getCharacterType(mobID), 0, 0, 0, 0, 0);	
		newMob->baseOnType(mobID);
		newMob->setSpawnInfo(x_pos, y_pos, respawn_rate, do_respawn);
		newMob->setAttitude(attitude);
		//newMob->setActiveGUI(&GUI);
		Globals::getCurrentZone()->addNPC(newMob);

		
	}

	InteractionRegion* addInteractionRegion(){
		InteractionRegion *newInteractionRegion = new InteractionRegion();
		Globals::getCurrentZone()->addInteractionRegion(newInteractionRegion);
		return newInteractionRegion;
	}

	void removeInteractionRegion(InteractionRegion *regionToRemove){
		regionToRemove->markAsDeletable();
	}

	InteractionPoint* addInteractionPoint(){
		InteractionPoint *newInteractionPoint = new InteractionPoint();
		Globals::getCurrentZone()->addInteractionPoint(newInteractionPoint);
		return newInteractionPoint;
	}

	InteractionPoint* addCharacterInteractionPoint(Character *character){
		InteractionPoint *newInteractionPoint = new CharacterInteractionPoint(character);
		Globals::getCurrentZone()->addInteractionPoint(newInteractionPoint);
		return newInteractionPoint;
	}

	void removeInteractionPoint(InteractionPoint *pointToRemove){
		//pointToRemove->markAsDeletable();
	}

	std::string getItemReferenceRestore(Character *character){
		if (character == NULL) {
			return "nil;";
		}
		for (std::map< std::string, Zone* >::iterator it = Globals::allZones.begin(); it != Globals::allZones.end(); ++it) {
			Zone *curZone = it->second;
			bool found;
			size_t foundPos;
			curZone->findCharacter(character, found, foundPos);
			if (found) {
				std::ostringstream oss;
				oss << "DawnInterface.restoreCharacterReference( \"" << curZone->getZoneName() << "\", " << foundPos << " )";
				return oss.str();
			}
		}
		// not found
	}

	TextWindow *createTextWindow(){
		TextWindow *newTextWindow = new TextWindow();
		Globals::allTextWindows.push_back(newTextWindow);
		newTextWindow->toggle();
		return newTextWindow;
	}
}