
#include <iostream>
#include "Zone.h"

#include "Constants.h"


bool TileMap::operator<(const TileMap& tile1) const{ 
	return tile->tileID < tile1.tile->tileID;
};

Zone::Zone() {
}

Zone::~Zone(){	
}

void Zone::loadZone(std::string file){
	m_name = file;

	bool needOwnTextureFrame = !Globals::initPhase;
	if (needOwnTextureFrame) {		
		Globals::initPhase = true;
		TextureAtlasCreator::get().init();
	}
	
	Globals::allZones[m_name] = this;
	LuaFunctions::executeLuaScript(std::string("DawnInterface.setCurrentZone( \"").append(m_name).append("\");"));
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
		TextureManager::SetTextureAtlas(m_name, TextureAtlasCreator::get().getAtlas());
		Globals::initPhase = false;
	}

	m_textureAtlas = &TextureManager::GetTextureAtlas(m_name);
}

std::string Zone::getName() const {
	return m_name;
}

bool Zone::zoneDataLoaded() const {
	return m_mapLoaded;
}

int Zone::locateTile(int x, int y){
	
	for (unsigned int t = 0; t < tileMap.size(); t++) {
		if ((tileMap[t].x_pos + tileMap[t].tile->texture.width > x) && (tileMap[t].x_pos < x)) {
			if ((tileMap[t].y_pos + tileMap[t].tile->texture.height > y) && (tileMap[t].y_pos < y)) {
				return t; 	
			}
		}
	}
	return -1;
}

void Zone::changeTile(int iId, Tile *tile_){
	if (iId >= 0) {
		tileMap[iId].tile = tile_;
	}
}

void Zone::deleteTile(int iId) {
	if (iId >= 0) {
		tileMap[iId].tile = EditorInterface::getTileSet(TileClassificationType::FLOOR)->getEmptyTile();
	}
}
void Zone::addEnvironment(int x_pos, int y_pos, Tile *tile, bool centeredOnPos){
	int placePosX = x_pos;
	int placePosY = y_pos;
	if (centeredOnPos) {
		placePosX -= tile->texture.width / 2;
		placePosY -= tile->texture.height / 2;
	}

	environmentMap.push_back(EnvironmentMap(placePosX, placePosY, tile, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0));

	if (tile->containsCollisionRect == true) {
		collisionMap.push_back({ placePosX + tile->collision.x,
											 placePosY + tile->collision.y,
											 tile->collision.w,
											 tile->collision.h });
	}
}

int Zone::deleteEnvironment(int x, int y){

	for (unsigned int t = 0; t<environmentMap.size(); t++) {
		if ((environmentMap[t].x_pos + environmentMap[t].tile->texture.width > x) && (environmentMap[t].x_pos < x)) {
			if ((environmentMap[t].y_pos + environmentMap[t].tile->texture.height > y) &&
				(environmentMap[t].y_pos < y)) {
				environmentMap.erase(environmentMap.begin() + t);
				return 0;
			}
		}
	}
	return 1;
}

void Zone::addShadow(int x_pos, int y_pos, Tile *tile) {
	shadowMap.push_back(EnvironmentMap(x_pos - (tile->texture.width / 2), y_pos - (tile->texture.height / 2), tile, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0));
}

int Zone::deleteShadow(int x, int y){

	for (unsigned int t = 0; t < shadowMap.size(); t++) {
		if ((shadowMap[t].x_pos + shadowMap[t].tile->texture.width > x) && (shadowMap[t].x_pos < x)) {
			if ((shadowMap[t].y_pos + shadowMap[t].tile->texture.height > y) &&
				(shadowMap[t].y_pos < y)) {
				shadowMap.erase(shadowMap.begin() + t);
				return 0;
			}
		}
	}
	return 1;
}

void Zone::drawZoneBatched() {
	
	Batchrenderer::get().setShader(Globals::shaderManager.getAssetPointer("batch"));
	glBindTexture(GL_TEXTURE_2D_ARRAY, *m_textureAtlas);

	drawTilesBatched();
	drawEnvironmentBatched();
	drawShadowsBatched();
	Batchrenderer::get().drawBuffer();

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

void Zone::drawTilesBatched() {
	for (unsigned int x = 0; x < tileMap.size(); x++) {
		TextureManager::DrawTextureBatched(tileMap[x].tile->texture, tileMap[x].x_pos, tileMap[x].y_pos);
	}
}

void Zone::drawEnvironmentBatched() {
	for (unsigned int x = 0; x < environmentMap.size(); x++) {
		TextureManager::DrawTextureBatched(environmentMap[x].tile->texture, environmentMap[x].x_pos, environmentMap[x].y_pos);
	}
}

void Zone::drawShadowsBatched(){
	for (unsigned int x = 0; x < shadowMap.size(); x++) {
		TextureManager::DrawTextureBatched(shadowMap[x].tile->texture, shadowMap[x].x_pos, shadowMap[x].y_pos);
	}
}

void Zone::drawZoneInstanced() {
	
	glBindTexture(GL_TEXTURE_2D_ARRAY, *m_textureAtlas);

	drawTilesInstanced();
	drawEnvironmentInstanced();

	Instancedrenderer::get().drawBuffer();

	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}


void Zone::drawTilesInstanced() {
	for (unsigned int x = 0; x < tileMap.size(); x++) {
		TextureManager::DrawTextureInstanced(tileMap[x].tile->texture, tileMap[x].x_pos, tileMap[x].y_pos);
	}
}

void Zone::drawEnvironmentInstanced() {
	for (unsigned int x = 0; x < environmentMap.size(); x++) {
		TextureManager::DrawTextureInstanced(environmentMap[x].tile->texture, environmentMap[x].x_pos, environmentMap[x].y_pos);
	}
}