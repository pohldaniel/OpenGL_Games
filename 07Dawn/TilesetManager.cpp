#include <iostream>
#include <memory>
#include <cassert>

#include "TilesetManager.h"

TileSetManager TileSetManager::s_instance;

TileSetManager& TileSetManager::Get() {
	return s_instance;
}

TileSet& TileSetManager::getTileSet(TileClassificationType::TileClassificationType tileType) {
	return m_tileSets[tileType];
}

void AdjacencyEquivalenceClass::addEquivalentTile(int tile, int offsetX, int offsetY){
	equivalentTiles.push_back(tile);
	offsets.push_back(Point(offsetX, offsetY));
}

AdjacencyEquivalenceClass *TileSet::createAdjacencyEquivalenceClass(){
	myEquivalenceClasses.push_back(new AdjacencyEquivalenceClass());
	return myEquivalenceClasses[myEquivalenceClasses.size() - 1];
}

TileSet::TileSet() {}

// The following functions are in the LUA EditorInterface
int TileSet::addTile(std::string filename, TileClassificationType::TileClassificationType tileType) {

	int tileId = tiles.size();
	std::auto_ptr<Tile> newTile = std::auto_ptr<Tile>(new Tile());

	newTile->tileID = tileId;
	newTile->texture = TextureManager::Loadimage(filename);
	Tile *tilePtr = newTile.release();
	tiles.push_back(tilePtr);

	return tileId;
}

int TileSet::addTileWithCollisionBox(std::string filename, TileClassificationType::TileClassificationType tileType, int cbx, int cby, int cbw, int cbh) {
	int tileId = addTile(filename, tileType);
	Tile *newTile = tiles[tileId];
	newTile->containsCollisionRect = true;
	newTile->collision = { cbx, cby, cbw,cbh };

	return tileId;
}

void TileSet::addAdjacency(int tile1, AdjacencyType::AdjacencyType adjacencyType, int tile2, int offsetX, int offsetY) {

	if (adjacencyType == AdjacencyType::LEFT) {
		addAdjacency(tile2, AdjacencyType::RIGHT, tile1, -offsetX, -offsetY);
		return;
	}

	if (adjacencyType == AdjacencyType::BOTTOM) {
		addAdjacency(tile2, AdjacencyType::TOP, tile1, -offsetX, -offsetY);
		return;
	}

	for (size_t curAdjacencyNr = 0; curAdjacencyNr<adjacencyList.size(); ++curAdjacencyNr) {
		AdjacencyStruct *curAdjacency = adjacencyList[curAdjacencyNr];
		if (curAdjacency->baseTile == tile1 && curAdjacency->adjacencyType == adjacencyType && curAdjacency->adjacentTile == tile2) {
			return;
		}
	}

	std::auto_ptr<AdjacencyStruct> newAdjacency = std::auto_ptr<AdjacencyStruct>(new AdjacencyStruct());
	newAdjacency->baseTile = tile1;
	newAdjacency->adjacentTile = tile2;
	newAdjacency->adjacencyType = adjacencyType;
	newAdjacency->offset = Point(offsetX, offsetY);
	adjacencyList.push_back(newAdjacency.release());
}

void TileSet::addEquivalenceAdjacency(AdjacencyEquivalenceClass *class1, AdjacencyType::AdjacencyType adjacencyType, AdjacencyEquivalenceClass *class2, int allOffsetX, int allOffsetY){

	std::vector<int> &firstTiles = class1->equivalentTiles;
	std::vector<int> &secondTiles = class2->equivalentTiles;
	for (size_t curFirstTileIndex = 0; curFirstTileIndex<firstTiles.size(); ++curFirstTileIndex) {
		for (size_t curSecondTileIndex = 0; curSecondTileIndex<secondTiles.size(); ++curSecondTileIndex) {
			addAdjacency(firstTiles[curFirstTileIndex], adjacencyType, secondTiles[curSecondTileIndex],
				class2->offsets[curSecondTileIndex].x - class1->offsets[curFirstTileIndex].x + allOffsetX,
				class2->offsets[curSecondTileIndex].y - class1->offsets[curFirstTileIndex].y + allOffsetY);
		}
	}
}

void TileSet::printTileSet() const{

	std::cout << "-- all " << tiles.size() << " Tiles in TileSet" << std::endl;
	for (size_t curTileNr = 0; curTileNr<tiles.size(); ++curTileNr) {
		Tile *curTile = tiles[curTileNr];
		std::cout << curTileNr << ": " << std::endl;
	}
	std::cout << std::endl;
	std::cout << "-- all " << adjacencyList.size() << " Adjacencies in TileSet" << std::endl;
	for (size_t curAdjacencyNr = 0; curAdjacencyNr<adjacencyList.size(); ++curAdjacencyNr) {
		AdjacencyStruct *curAdjacency = adjacencyList[curAdjacencyNr];
		Tile *startTile = tiles[curAdjacency->baseTile];
		Tile *endTile = tiles[curAdjacency->adjacentTile];
		if (curAdjacency->adjacencyType == AdjacencyType::RIGHT) {
			std::cout << startTile->tileID << " [on right] <--> [on left] " << endTile->tileID << std::endl;
		}
		else {
			std::cout << startTile->tileID << " [on top] <--> [on bottom] " << endTile->tileID << std::endl;
		}
	}
	std::cout << std::endl;
}

Tile* TileSet::getTile(int tileID) const{
	return tiles[tileID];
}

Tile* TileSet::getEmptyTile() const{
	return tiles[0];
}

unsigned int TileSet::numberOfTiles() const{
	return tiles.size();
}

void TileSet::clear(){
	tiles.clear();
}

std::vector<Tile*> TileSet::getAllTiles() const{
	return tiles;
}

void TileSet::getAllAdjacentTiles(Tile *searchTile, std::vector< std::vector<Tile*> > &result, std::vector< std::vector<Point> > &matchOffsets) const{
	result.clear();
	result.resize(4);
	matchOffsets.clear();
	matchOffsets.resize(4);

	for (size_t curAdjacencyNr = 0; curAdjacencyNr<adjacencyList.size(); ++curAdjacencyNr) {
		AdjacencyStruct *curAdjacency = adjacencyList[curAdjacencyNr];
		if (tiles[curAdjacency->baseTile] == searchTile) {
			if (curAdjacency->adjacencyType == AdjacencyType::RIGHT) {
				result[AdjacencyType::RIGHT].push_back(tiles[curAdjacency->adjacentTile]);
				matchOffsets[AdjacencyType::RIGHT].push_back(Point(curAdjacency->offset.x, curAdjacency->offset.y));
			}
			else {
				assert(curAdjacency->adjacencyType == AdjacencyType::TOP);
				result[AdjacencyType::TOP].push_back(tiles[curAdjacency->adjacentTile]);
				matchOffsets[AdjacencyType::TOP].push_back(Point(curAdjacency->offset.x, curAdjacency->offset.y));
			}
		}
		if (tiles[curAdjacency->adjacentTile] == searchTile) {
			if (curAdjacency->adjacencyType == AdjacencyType::RIGHT) {
				result[AdjacencyType::LEFT].push_back(tiles[curAdjacency->baseTile]);
				matchOffsets[AdjacencyType::LEFT].push_back(Point(-curAdjacency->offset.x, -curAdjacency->offset.y));
			}
			else {
				assert(curAdjacency->adjacencyType == AdjacencyType::TOP);
				result[AdjacencyType::BOTTOM].push_back(tiles[curAdjacency->baseTile]);
				matchOffsets[AdjacencyType::BOTTOM].push_back(Point(-curAdjacency->offset.x, -curAdjacency->offset.y));
			}
		}
	}
}

namespace EditorInterface{

	TileSet *getTileSet(TileClassificationType::TileClassificationType tileType) {
		return &TileSetManager::Get().getTileSet(tileType);
	}
}