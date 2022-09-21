
#include <iostream>
#include "Zone.h"


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
		TextureAtlasCreator::get().init(m_textureAtlas);
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
		TextureAtlasCreator::get().shutdown();
		Globals::initPhase = false;
	}
}

bool Zone::zoneDataLoaded() const {
	return m_mapLoaded;
}

int Zone::locateTile(int x, int y){
	
	for (unsigned int t = 0; t < TileMap.size(); t++) {
		if ((TileMap[t].x_pos + TileMap[t].tile->texture.width > x) && (TileMap[t].x_pos < x)) {
			if ((TileMap[t].y_pos + TileMap[t].tile->texture.height > y) && (TileMap[t].y_pos < y)) {
				return t; 	
			}
		}
	}
	return -1;

}

void Zone::changeTile(int iId, Tile *tile_){
	if (iId >= 0) {
		TileMap[iId].tile = tile_;
	}
}

void Zone::deleteTile(int iId) {
	if (iId >= 0) {
		TileMap[iId].tile = EditorInterface::getTileSet(TileClassificationType::FLOOR)->getEmptyTile();
	}
}
void Zone::addEnvironment(int x_pos, int y_pos, Tile *tile, bool centeredOnPos){
	int placePosX = x_pos;
	int placePosY = y_pos;
	if (centeredOnPos) {
		placePosX -= tile->texture.width / 2;
		placePosY -= tile->texture.height / 2;
	}

	EnvironmentMap.push_back(sEnvironmentMap(placePosX, placePosY, tile,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0));
	/**if (tile->texture->getTexture(0).contains_collision_box == true) {
		CollisionMap.push_back(sCollisionMap(placePosX + tile->texture->getTexture(0).collision_box.x,
			placePosY + tile->texture->getTexture(0).collision_box.y,
			tile->texture->getTexture(0).collision_box.h,
			tile->texture->getTexture(0).collision_box.w));
	}*/
}

int Zone::deleteEnvironment(int x, int y){

	for (unsigned int t = 0; t<EnvironmentMap.size(); t++) {
		if ((EnvironmentMap[t].x_pos + EnvironmentMap[t].tile->texture.width > x) && (EnvironmentMap[t].x_pos < x)) {
			if ((EnvironmentMap[t].y_pos + EnvironmentMap[t].tile->texture.height > y) &&
				(EnvironmentMap[t].y_pos < y)) {
				EnvironmentMap.erase(EnvironmentMap.begin() + t);
				return 0;
			}
		}
	}
	return 1;
}

void Zone::addShadow(int x_pos, int y_pos, Tile *tile) {
	ShadowMap.push_back(sEnvironmentMap(x_pos - (tile->texture.width / 2), y_pos - (tile->texture.height / 2), tile, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0));
}

int Zone::deleteShadow(int x, int y){

	for (unsigned int t = 0; t<ShadowMap.size(); t++) {
		if ((ShadowMap[t].x_pos + ShadowMap[t].tile->texture.width > x) && (ShadowMap[t].x_pos < x)) {
			if ((ShadowMap[t].y_pos + ShadowMap[t].tile->texture.height > y) &&
				(ShadowMap[t].y_pos < y)) {
				ShadowMap.erase(ShadowMap.begin() + t);
				return 0;
			}
		}
	}
	return 1;
}

void Zone::drawZoneBatched() {
	
	Batchrenderer::get().setShader(Globals::shaderManager.getAssetPointer("batch"));
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureAtlas);

	drawTilesBatched();
	drawEnvironmentBatched();
	drawShadowsBatched();
	Batchrenderer::get().drawBuffer();

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Zone::drawTilesBatched() {
	for (unsigned int x = 0; x < TileMap.size(); x++) {
		TextureManager::DrawTextureBatched(TileMap[x].tile->texture, TileMap[x].x_pos, TileMap[x].y_pos);
	}
}

void Zone::drawEnvironmentBatched() {
	for (unsigned int x = 0; x < EnvironmentMap.size(); x++) {
		TextureManager::DrawTextureBatched(EnvironmentMap[x].tile->texture, EnvironmentMap[x].x_pos, EnvironmentMap[x].y_pos);
	}
}

void Zone::drawShadowsBatched(){
	for (unsigned int x = 0; x < ShadowMap.size(); x++) {
		TextureManager::DrawTextureBatched(ShadowMap[x].tile->texture, ShadowMap[x].x_pos, ShadowMap[x].y_pos);
	}
}

void Zone::drawZoneInstanced() {
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_textureAtlas);

	drawTilesInstanced();
	drawEnvironmentInstanced();

	Instancedrenderer::get().drawBuffer();

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}


void Zone::drawTilesInstanced() {
	for (unsigned int x = 0; x < TileMap.size(); x++) {
		TextureManager::DrawTextureInstanced(TileMap[x].tile->texture, TileMap[x].x_pos, TileMap[x].y_pos);
	}
}

void Zone::drawEnvironmentInstanced() {
	for (unsigned int x = 0; x < EnvironmentMap.size(); x++) {
		TextureManager::DrawTextureInstanced(EnvironmentMap[x].tile->texture, EnvironmentMap[x].x_pos, EnvironmentMap[x].y_pos);
	}
}