#pragma once
#include <string>
#include <vector>
#include <array>
#include <unordered_map>

#include "Enums.h"
#include "TextureManager.h"
#include "Character.h"

struct CollisionRect {
	int x, y, w, h;	
};

class Tile{
public:
	unsigned int tileId;
	bool containsCollisionRect;
	TextureRect textureRect;
	CollisionRect collisionRect;
};

struct AdjacencyStruct{
	Enums::AdjacencyType adjacencyType;
	unsigned int baseTile;
	unsigned int adjacentTile;
	std::array<int,2> offset;
};

struct AdjacencyEquivalenceClass{
	std::vector<int> equivalentTiles;
	std::vector<std::array<int,2>> offsets;
	void addEquivalentTile(int tile, int offsetX, int offsetY);
};

class TileSet{
public:
	
	TileSet();

	// The following functions are in the LUA EditorInterface
	unsigned int addTile(std::string filename, Enums::TileClassificationType tileType);
	unsigned int addTileWithCollisionBox(std::string filename, Enums::TileClassificationType tileType, int cbx, int cby, int cbw, int cbh);
	void addEquivalenceAdjacency(AdjacencyEquivalenceClass *class1, Enums::AdjacencyType adjacencyType, AdjacencyEquivalenceClass *class2, int allOffsetX, int allOffsetY);
	void addAdjacency(unsigned int tile1, Enums::AdjacencyType adjacencyType, unsigned int tile2, int offsetX, int offsetY);
	AdjacencyEquivalenceClass* createAdjacencyEquivalenceClass();	
	void printTileSet() const;

	// normal interface
	const Tile& getTile(int tileID);
	const Tile& getEmptyTile();
	const std::vector<Tile>& getAllTiles() const;
	void getAllAdjacentTiles(const Tile& searchTile, std::vector<std::vector<Tile>> &matchingTiles, std::vector<std::vector<std::array<int,2>>> &matchOffsets) const;
	unsigned int numberOfTiles();
	void clearTiles();
	
private:
	std::vector<AdjacencyStruct> m_adjacencyList;
	std::vector<AdjacencyEquivalenceClass*> m_equivalenceClasses;
	std::vector<Tile> m_tiles;
};

class TileSetManager {

public:

	TileSet& getTileSet(Enums::ActivityType activityType, Enums::Direction direction);
	TileSet& getTileSet(Enums::TileClassificationType tileType);
	static TileSetManager& Get();
	
private:
	TileSetManager() = default;
	
	std::unordered_map<int, TileSet> m_tileSets;
	std::unordered_map<std::pair<int, int>, TileSet, pair_hash> m_moveTileSets;

	unsigned int textureAtlas;
	static TileSetManager s_instance;
};

