#include "Luainterface.h"

#include "TilesetManager.h"
#include "Zone.h"

#include "Constants.h"
#include "Npc.h"

namespace EditorInterface{
	void addGroundTile(int posX, int posY, int tile){		
		Zone *currentZone = Globals::getCurrentZone();
		TileSet *tileSet = EditorInterface::getTileSet(TileClassificationType::TileClassificationType::FLOOR);
		currentZone->tileMap.push_back(TileMap(posX, posY, tileSet->getTile(tile)));
	}

	void addEnvironment(int posX, int posY, int posZ, int tile){
		Zone *currentZone = Globals::getCurrentZone();
		TileSet *tileSet = EditorInterface::getTileSet(TileClassificationType::TileClassificationType::ENVIRONMENT);
		currentZone->environmentMap.push_back(EnvironmentMap(posX, posY, tileSet->getTile(tile), 1, 1, 1, 1, 1, 1, posZ));
	}

	void adjustLastRGBA(double red, double green, double blue, double alpha){
		Zone *currentZone = Globals::getCurrentZone();
		EnvironmentMap &lastEnv = currentZone->environmentMap[currentZone->environmentMap.size() - 1];
		lastEnv.red = red;
		lastEnv.green = green;
		lastEnv.blue = blue;
		lastEnv.transparency = alpha;
	}

	/*void adjustLastScale(double scaleX, double scaleY){
		CZone *currentZone = Globals::getCurrentZone();
		sEnvironmentMap &lastEnv = currentZone->EnvironmentMap[currentZone->EnvironmentMap.size() - 1];
		lastEnv.x_scale = scaleX;
		lastEnv.y_scale = scaleY;
	}*/

	void addCollisionRect(int lrx, int lry, int width, int height){
		Zone *currentZone = Globals::getCurrentZone();
		currentZone->collisionMap.push_back({ lrx, lry, width, height });
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
		if (zoneNameNoPrefix.find_last_of('/') != std::string::npos)
		{
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

	Character* createNewMobType(std::string typeID){
		Character* newMobType = new Npc(0, 0, 0, 0, 0);
		Globals::allMobTypes[typeID] = newMobType;
		return newMobType;
	}

	Npc* addMobSpawnPoint(std::string mobID, int x_pos, int y_pos, int respawn_rate, int do_respawn) {
		Npc* newMob = new Npc(0, 0, 0, 0, 0);
		//newMob->baseOnType(mobID);

		if (Globals::allMobTypes.count(mobID) != 1) {
			return newMob;
		}

		Character* other = Globals::allMobTypes[mobID];

		newMob->setTileSet(other->m_moveTileSets);

		
		
		newMob->setSpawnInfo(x_pos, y_pos, respawn_rate, do_respawn);
		//newMob->setActiveGUI(&GUI);
		Globals::getCurrentZone()->addNPC(newMob);
		return newMob;
	}
}