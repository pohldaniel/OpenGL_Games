#pragma once
#include <Box2D\Box2D.h>
#include "nlohmann\json.hpp"

#include <iostream>
#include "engine\Vector.h"
#include "engine\Shader.h"
#include "engine\Spritesheet.h"

#include "Constants.h"

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
	Level();
	~Level();

	void loadFile(const std::string &file);
	void loadLayers(nlohmann::json &map);
	void loadTileLayer(nlohmann::json &layer_in, TileLayer &layer_out, bool reverse = true);
	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices);
	void render();

	bool contourTrace(const std::vector<Tile> &grid, std::vector<bool> &visited);
	void contourTraceComponent(int startX, int startY, int startBacktrackX, int startBacktrackY, const std::vector<Tile> &grid, std::vector<bool> &visited);
	std::vector<std::pair<int, int>> getMooreNeighborhood(int startX, int startY);
	int getMooreIndex(int boundaryX, int boundaryY, int neighborX, int neighborY);
	void addChainVertex(int startX, int startY, int previousX, int previousY, std::vector<Vector2f> &chainVertices);
	void createStaticBody(std::vector<Vector2f> &chainVertices);

	std::vector<bool> m_bitVector;

	//b2World* getWorld() const;

	std::vector<b2ChainShape> m_chain;
	std::vector<b2Body*> m_contours;
	b2Body *slopeBody;
	float slopeAngel = 20.0f;
	b2Vec2 slopePosition = b2Vec2(500.0f, 500.0f);
private:
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	TileLayer m_layer;
	//b2World * m_world;

	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_map;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;
};