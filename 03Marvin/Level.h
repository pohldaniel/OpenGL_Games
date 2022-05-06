#pragma once
#include <Box2D\Box2D.h>
#include "nlohmann\json.hpp"

#include <iostream>
#include "engine\Vector.h"
#include "engine\Shader.h"
#include "engine\Spritesheet.h"

#include "CollisionHandler.h"
#include "Constants.h"
#include "MovingPlatform.h"

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
};

class Level {
	
public:
	Level(const float& dt, const float& fdt);
	~Level();

	void loadFile(const std::string &file);
	void loadLayers(nlohmann::json &map);
	void loadTileLayer(nlohmann::json &layer_in, TileLayer &layer_out, bool reverse = true);
	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices);
	void render();
	void fixedUpdate();
	void update();

	bool contourTrace(const std::vector<Tile> &grid, std::vector<bool> &visited);
	void contourTraceComponent(int startX, int startY, int startBacktrackX, int startBacktrackY, const std::vector<Tile> &grid, std::vector<bool> &visited);
	std::vector<std::pair<int, int>> getMooreNeighborhood(int startX, int startY);
	int getMooreIndex(int boundaryX, int boundaryY, int neighborX, int neighborY);
	void addChainVertex(int startX, int startY, int previousX, int previousY, std::vector<Vector2f> &chainVertices);
	void createStaticBody(std::vector<Vector2f> &chainVertices);

	std::vector<bool> m_bitVector;
	std::vector<b2Body*> m_contours;
	
private:
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	TileLayer m_layer;
	
	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_indexMap;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;

	const float& m_fdt;
	const float& m_dt;
};