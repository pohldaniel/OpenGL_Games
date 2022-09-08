#include "Luainterface.h"


namespace EditorInterface{
	void addGroundTile(int posX, int posY, int tile){		
		Zone *currentZone = Globals::getCurrentZone();
		TileSet *tileSet = EditorInterface::getTileSet();
		currentZone->TileMap.push_back(sTileMap(posX, posY, tileSet->getTile(tile)));
	}

	void addEnvironment(int posX, int posY, int posZ, int tile){
		Zone *currentZone = Globals::getCurrentZone();
		TileSet *tileSet = EditorInterface::getTileSet();
		currentZone->EnvironmentMap.push_back(sEnvironmentMap(posX, posY, tileSet->getTile(tile), 1, 1, 1, 1, 1, 1, posZ));
	}

	void adjustLastRGBA(double red, double green, double blue, double alpha){
		Zone *currentZone = Globals::getCurrentZone();
		sEnvironmentMap &lastEnv = currentZone->EnvironmentMap[currentZone->EnvironmentMap.size() - 1];
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
		currentZone->CollisionMap.push_back(sCollisionMap(lrx, lry, height, width));
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
}