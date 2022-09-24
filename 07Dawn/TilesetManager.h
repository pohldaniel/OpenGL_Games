#pragma once
#include <string>
#include <vector>

#include "TextureManager.h"
#include "Character.h"

namespace TileClassificationType{
	enum TileClassificationType{
		FLOOR,
		ENVIRONMENT,
		SHADOW,
		COLLISION,
		NPC,
		COUNT
	};
}

namespace AdjacencyType{
	enum AdjacencyType{
		RIGHT,
		LEFT,
		TOP,
		BOTTOM
	};
}

struct CollisionRect {
	int x, y, w, h;	
};

class Tile{
public:
	int tileID;
	bool containsCollisionRect;
	TextureRect textureRect;
	CollisionRect collisionRect;
};

struct Point{
	int x;
	int y;

	Point(int x_ = 0, int y_ = 0) : x(x_), y(y_){
	}
};

class AdjacencyStruct{
public:
	AdjacencyType::AdjacencyType adjacencyType;
	int baseTile;
	int adjacentTile;
	Point offset;
};

class AdjacencyEquivalenceClass{
public:
	std::vector<int> equivalentTiles;
	std::vector<Point> offsets;
	void addEquivalentTile(int tile, int offsetX, int offsetY);
};

class TileSet{
public:
	std::vector<Tile*> tiles;
	std::vector<AdjacencyStruct*> adjacencyList;
	std::vector<AdjacencyEquivalenceClass*> myEquivalenceClasses;


	TileSet();

	// The following functions are in the LUA EditorInterface
	int addTile(std::string filename, TileClassificationType::TileClassificationType tileType);
	int addTileWithCollisionBox(std::string filename, TileClassificationType::TileClassificationType tileType, int cbx, int cby, int cbw, int cbh);
	void addAdjacency(int tile1, AdjacencyType::AdjacencyType adjacencyType, int tile2, int offsetX, int offsetY);
	AdjacencyEquivalenceClass *createAdjacencyEquivalenceClass();
	void addEquivalenceAdjacency(AdjacencyEquivalenceClass *class1, AdjacencyType::AdjacencyType adjacencyType, AdjacencyEquivalenceClass *class2, int allOffsetX, int allOffsetY);
	void printTileSet() const;

	// normal interface
	Tile *getTile(int tileID) const;
	Tile *getEmptyTile() const;
	unsigned int numberOfTiles() const;
	void clear();
	std::vector<Tile*> getAllTiles() const;

	void getAllAdjacentTiles(Tile *searchTile, std::vector< std::vector<Tile*> > &matchingTiles, std::vector< std::vector<Point> > &matchOffsets) const;

};

class TileSetManager {

public:

	TileSet& getTileSet(ActivityType::ActivityType activityType, Direction direction);
	TileSet& getTileSet(TileClassificationType::TileClassificationType tileType);	
	static TileSetManager& Get();
	
private:
	TileSetManager() = default;
	
	std::map<int, TileSet> m_tileSets;
	std::map<std::pair<int, int>, TileSet> m_moveTileSets;

	unsigned int textureAtlas;
	static TileSetManager s_instance;
};

