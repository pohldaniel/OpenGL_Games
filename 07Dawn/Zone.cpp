
#include <iostream>
#include "Zone.h"

#include "luafunctions.h"
#include "Constants.h"

bool sTileMap::operator<(const sTileMap& tile1) const{ 
	// instead of using a predicate in our sort call.
	return tile->tileID < tile1.tile->tileID;
};

Zone::Zone(){
}

Zone::~Zone(){	
}

void Zone::loadZone(std::string file){
	
	bool needOwnTextureFrame = !Globals::initPhase;
	if (needOwnTextureFrame) {
		Globals::initPhase = true;
	}

	m_zoneName = file;


	Globals::allZones[m_zoneName] = this;
	LuaFunctions::executeLuaScript(std::string("DawnInterface.setCurrentZone( \"").append(m_zoneName).append("\");"));
	LuaFunctions::executeLuaScript(std::string("if ( MapData == nil )\nthen\n    MapData={}\nend"));
	LuaFunctions::executeLuaScript(std::string("if ( MapData.DontSave == nil )\nthen\n    MapData.DontSave={}\nend"));

	LuaFunctions::executeLuaFile(std::string(file).append(".tiles_ground.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".tiles_environment.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".tiles_shadow.lua"));

	LuaFunctions::executeLuaFile(std::string(file).append(".ground.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".environment.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".shadow.lua"));
	LuaFunctions::executeLuaFile(std::string(file).append(".collision.lua"));

	//LuaFunctions::executeLuaFile(std::string(file).append(".init.lua"));

	m_mapLoaded = true;

	if (needOwnTextureFrame) {
		CTexture::s_textureAtlas.finishFrame();
		Globals::initPhase = false;
	}
}

bool Zone::zoneDataLoaded() const {
	return m_mapLoaded;
}

void Zone::addEnvironment(int x_pos, int y_pos, Tile *tile, bool centeredOnPos){
	int placePosX = x_pos;
	int placePosY = y_pos;
	/**if (centeredOnPos) {
		placePosX -= tile->texture->getTexture(0).width / 2;
		placePosY -= tile->texture->getTexture(0).height / 2;
	}*/

	EnvironmentMap.push_back(sEnvironmentMap(placePosX, placePosY, tile,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0));
	/**if (tile->texture->getTexture(0).contains_collision_box == true) {
		CollisionMap.push_back(sCollisionMap(placePosX + tile->texture->getTexture(0).collision_box.x,
			placePosY + tile->texture->getTexture(0).collision_box.y,
			tile->texture->getTexture(0).collision_box.h,
			tile->texture->getTexture(0).collision_box.w));
	}*/
}

void Zone::drawTiles() {
	for (unsigned int x = 0; x < TileMap.size(); x++) {
		TileMap[x].tile->texture->DrawTexture(TileMap[x].x_pos, TileMap[x].y_pos, 0);
	}
}

void Zone::drawEnvironment() {
	for (unsigned int x = 0; x < EnvironmentMap.size(); x++) {
		EnvironmentMap[x].tile->texture->DrawTexture(EnvironmentMap[x].x_pos,
													 EnvironmentMap[x].y_pos, 
													 0,
													 EnvironmentMap[x].transparency, EnvironmentMap[x].red,
													 EnvironmentMap[x].green, EnvironmentMap[x].blue,
													 EnvironmentMap[x].x_scale, EnvironmentMap[x].y_scale);
	}
}