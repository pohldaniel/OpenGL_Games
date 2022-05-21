#pragma once

#include <Box2D\Box2D.h>
#include "nlohmann\json.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#include "engine\Vector.h"

#include "Object.h"
#include "Entity.h"
#include "Barnacle.h"
#include "Bee.h"
#include "Slime.h"
#include "GrassBlock.h"
#include "SnakeSlime.h"

struct Tile {
	Vector2f position;
	Vector2f size;
	unsigned int gid;
};

struct TileLayer {

public:
	std::vector<Tile> tiles;
	std::vector<Tile> tilesCol;

	std::string name;
	bool visible;
	double opacity;
	unsigned int width;
	unsigned int height;
	bool collisionLayer = false;
};

struct JSONObject {
	std::string type;
	std::string name;
	Vector2f position;
	unsigned int posX;
	unsigned int posY;
};

struct MapLoader {

public:
	
	static MapLoader& get();

	void loadLevel(const std::string &file);
	void setDelta(const float& dt, const float& fdt);

	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_indexMap;
	std::vector<b2Body*> m_contours;
	std::vector<b2Body*> m_phyObjects;
	std::vector<RenderableObject*> m_renObjects;
	std::vector<Entity*> m_entities2;

	Vector2f m_playerPosition;

private:

	std::vector<JSONObject> m_objects;
	std::vector<JSONObject> m_entities;
	std::vector<TileLayer> m_layers;
	std::vector<bool> m_bitVector;

	void loadLayers(nlohmann::json &map);
	void loadObjects(nlohmann::json &map);
	void loadTileLayer(nlohmann::json &layer_in, TileLayer &layer_out, bool reverse = true);
	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices);

	void createPhysicsBody(JSONObject &object);
	void createEntity(JSONObject &object);

	bool contourTrace(const std::vector<Tile> &grid, std::vector<bool> &visited, const TileLayer& layer);
	void contourTraceComponent(int startX, int startY, int startBacktrackX, int startBacktrackY, const std::vector<Tile> &grid, std::vector<bool> &visited, const TileLayer& layer);
	std::vector<std::pair<int, int>> getMooreNeighborhood(int startX, int startY, const TileLayer& layer);
	int getMooreIndex(int boundaryX, int boundaryY, int neighborX, int neighborY);
	void addChainVertex(int startX, int startY, int previousX, int previousY, std::vector<Vector2f> &chainVertices, const TileLayer& layer);
	void createStaticBody(std::vector<Vector2f> &chainVertices);

	static MapLoader s_instance;
	const float* m_fdt;
	const float* m_dt;

	unsigned int m_width;
	unsigned int m_height;
};