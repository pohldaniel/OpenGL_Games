#pragma once
#include <Box2D\Box2D.h>
#include "nlohmann\json.hpp"

#include <iostream>
#include "engine\Vector.h"
#include "engine\Shader.h"
#include "engine\Spritesheet.h"
#include "engine\Quad.h"

#include "CollisionHandler.h"
#include "Object.h"
#include "Entity.h"
#include "Constants.h"
#include "MovingPlatform.h"
#include "ViewEffect.h"

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

class Level {
	
public:
	Level(const float& dt, const float& fdt);
	~Level();

	void loadFile(const std::string &file);
	void loadLayers(nlohmann::json &map);
	void loadObjects(nlohmann::json &map);
	void loadTileLayer(nlohmann::json &layer_in, TileLayer &layer_out, bool reverse = true);
	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices);
	void render();
	void fixedUpdate();
	void update();

	bool contourTrace(const std::vector<Tile> &grid, std::vector<bool> &visited, const TileLayer& layer);
	void contourTraceComponent(int startX, int startY, int startBacktrackX, int startBacktrackY, const std::vector<Tile> &grid, std::vector<bool> &visited, const TileLayer& layer);
	std::vector<std::pair<int, int>> getMooreNeighborhood(int startX, int startY, const TileLayer& layer);
	int getMooreIndex(int boundaryX, int boundaryY, int neighborX, int neighborY);
	void addChainVertex(int startX, int startY, int previousX, int previousY, std::vector<Vector2f> &chainVertices, const TileLayer& layer);
	void createStaticBody(std::vector<Vector2f> &chainVertices);

	std::vector<bool> m_bitVector;
	std::vector<b2Body*> m_contours;
	std::vector<b2Body*> m_phyObjects;

private:
	Shader *m_shader;
	Quad *m_quadBackground;
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	std::vector<TileLayer> m_layers;
	std::unordered_map<std::string, unsigned int> m_sprites;

	std::vector<JSONObject> m_objects;

	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_indexMap;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;

	const float& m_fdt;
	const float& m_dt;


	b2Body* createPhysicsBody(JSONObject &object);
};