#include <iostream>
#include <memory>
#include <cassert>

#include "TilesetManager.h"

TileSetManager TileSetManager::s_instance;

TileSetManager& TileSetManager::Get() {
	return s_instance;
}

TileSet& TileSetManager::getTileSet(Enums::TileClassificationType tileType) {
	return m_tileSets[tileType];
}

TileSet& TileSetManager::getTileSet(Enums::ActivityType activityType, Enums::Direction direction) {
	return m_moveTileSets[{activityType, direction}];
}

void AdjacencyEquivalenceClass::addEquivalentTile(int tile, int offsetX, int offsetY){
	equivalentTiles.push_back(tile);
	offsets.push_back({ offsetX, offsetY });
}

AdjacencyEquivalenceClass* TileSet::createAdjacencyEquivalenceClass(){

	m_equivalenceClasses.push_back(new AdjacencyEquivalenceClass());
	return m_equivalenceClasses[m_equivalenceClasses.size() - 1];
}

TileSet::TileSet() {}

// The following functions are in the LUA EditorInterface
unsigned int TileSet::addTile(std::string filename, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {

	unsigned int tileId = static_cast<unsigned int>(m_tiles.size());

	m_tiles.push_back({ tileId , false, TextureManager::Loadimage(filename, 0, 0, paddingLeft, paddingRight, paddingTop, paddingBottom), {} });
	return tileId;
}

unsigned int TileSet::addTile(std::string filename, unsigned int maxWidth, unsigned int maxHeight, int paddingLeft, int paddingRight, int paddingTop, int paddingBottom) {

	unsigned int tileId = static_cast<unsigned int>(m_tiles.size());

	m_tiles.push_back({ tileId , false, TextureManager::Loadimage(filename, maxWidth, maxHeight, paddingLeft, paddingRight, paddingTop, paddingBottom), {} });
	return tileId;
}

unsigned int TileSet::addTileWithCollisionBox(std::string filename, int cbx, int cby, int cbw, int cbh) {
	unsigned int tileId = addTile(filename);
	Tile& newTile = m_tiles[tileId];
	newTile.containsCollisionRect = true;
	newTile.collisionRect = { cbx, cby, cbw,cbh };

	return tileId;
}

unsigned int TileSet::getOffset() const {
	return m_offset;
}

void TileSet::setOffset(unsigned int offset) {
	m_offset = offset;
}

void TileSet::addAdjacency(unsigned int tile1, Enums::AdjacencyType adjacencyType, unsigned int tile2, int offsetX, int offsetY) {

	if (adjacencyType == Enums::LEFT) {
		addAdjacency(tile2, Enums::RIGHT, tile1, -offsetX, -offsetY);
		return;
	}

	if (adjacencyType == Enums::BOTTOM) {
		addAdjacency(tile2, Enums::TOP, tile1, -offsetX, -offsetY);
		return;
	}

	for (size_t curAdjacencyNr = 0; curAdjacencyNr<m_adjacencyList.size(); ++curAdjacencyNr) {
		AdjacencyStruct curAdjacency = m_adjacencyList[curAdjacencyNr];
		if (curAdjacency.baseTile == tile1 && curAdjacency.adjacencyType == adjacencyType && curAdjacency.adjacentTile == tile2) {
			return;
		}
	}
	m_adjacencyList.push_back({ adjacencyType, tile1, tile2, { offsetX, offsetY }});
}

void TileSet::addEquivalenceAdjacency(AdjacencyEquivalenceClass *class1, Enums::AdjacencyType adjacencyType, AdjacencyEquivalenceClass *class2, int allOffsetX, int allOffsetY){
	
	std::vector<int> &firstTiles = class1->equivalentTiles;
	std::vector<int> &secondTiles = class2->equivalentTiles;


	for (size_t curFirstTileIndex = 0; curFirstTileIndex<firstTiles.size(); ++curFirstTileIndex) {
		for (size_t curSecondTileIndex = 0; curSecondTileIndex<secondTiles.size(); ++curSecondTileIndex) {
			addAdjacency(firstTiles[curFirstTileIndex], adjacencyType, secondTiles[curSecondTileIndex],
				class2->offsets[curSecondTileIndex][0] - class1->offsets[curFirstTileIndex][0] + allOffsetX,
				class2->offsets[curSecondTileIndex][1] - class1->offsets[curFirstTileIndex][1] + allOffsetY);
		}
	}
}

void TileSet::printTileSet() const{

	std::cout << "-- all " << m_tiles.size() << " Tiles in TileSet" << std::endl;
	for (size_t curTileNr = 0; curTileNr < m_tiles.size(); ++curTileNr) {
		const Tile& curTile = m_tiles[curTileNr];
		std::cout << curTileNr << ": " << std::endl;
	}
	std::cout << std::endl;
	std::cout << "-- all " << m_adjacencyList.size() << " Adjacencies in TileSet" << std::endl;
	for (size_t curAdjacencyNr = 0; curAdjacencyNr < m_adjacencyList.size(); ++curAdjacencyNr) {
		const AdjacencyStruct& curAdjacency = m_adjacencyList[curAdjacencyNr];
		const Tile& startTile = m_tiles[curAdjacency.baseTile];
		const Tile& endTile = m_tiles[curAdjacency.adjacentTile];
		if (curAdjacency.adjacencyType == Enums::AdjacencyType::RIGHT) {
			std::cout << startTile.tileId << " [on right] <--> [on left] " << endTile.tileId << std::endl;
		}else {
			std::cout << startTile.tileId << " [on top] <--> [on bottom] " << endTile.tileId << std::endl;
		}
	}
	std::cout << std::endl;
}

const Tile& TileSet::getTile(int tileID)  const{
	return m_tiles[tileID];
}

const Tile& TileSet::getEmptyTile() {
	return m_tiles[0];
}

unsigned int TileSet::numberOfTiles() {
	return static_cast<unsigned int>(m_tiles.size());
}

void TileSet::clearTiles(){
	m_tiles.clear();
}

const std::vector<Tile>& TileSet::getAllTiles() const{
	return m_tiles;
}

void TileSet::getAllAdjacentTiles(const Tile& searchTile, std::vector< std::vector<Tile> > &result, std::vector< std::vector<std::array<int, 2>> > &matchOffsets) const{
	result.clear();
	result.resize(4);
	matchOffsets.clear();
	matchOffsets.resize(4);

	for (size_t curAdjacencyNr = 0; curAdjacencyNr < m_adjacencyList.size(); ++curAdjacencyNr) {
		const AdjacencyStruct& curAdjacency = m_adjacencyList[curAdjacencyNr];
		if (m_tiles[curAdjacency.baseTile].tileId == searchTile.tileId) {
			if (curAdjacency.adjacencyType == Enums::AdjacencyType::RIGHT) {
				result[Enums::AdjacencyType::RIGHT].push_back(m_tiles[curAdjacency.adjacentTile]);
				matchOffsets[Enums::AdjacencyType::RIGHT].push_back({ curAdjacency.offset[0], curAdjacency.offset[1] });

			}else {
				result[Enums::AdjacencyType::TOP].push_back(m_tiles[curAdjacency.adjacentTile]);
				matchOffsets[Enums::AdjacencyType::TOP].push_back({ curAdjacency.offset[0], curAdjacency.offset[1] });
			}
		}

		if (m_tiles[curAdjacency.adjacentTile].tileId == searchTile.tileId) {
			if (curAdjacency.adjacencyType == Enums::AdjacencyType::RIGHT) {
				result[Enums::AdjacencyType::LEFT].push_back(m_tiles[curAdjacency.baseTile]);
				matchOffsets[Enums::AdjacencyType::LEFT].push_back({ -curAdjacency.offset[0], -curAdjacency.offset[1] });
			}else {
				result[Enums::AdjacencyType::BOTTOM].push_back(m_tiles[curAdjacency.baseTile]);
				matchOffsets[Enums::AdjacencyType::BOTTOM].push_back({ -curAdjacency.offset[0], -curAdjacency.offset[1] });
			}
		}
	}
}