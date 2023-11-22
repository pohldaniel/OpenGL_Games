#pragma once
#include <Box2D\Box2D.h>
#include "nlohmann\json.hpp"

#include <iostream>
#include <fstream>
#include "engine\Vector.h"
#include "engine\Extension.h"

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

	std::vector<b2ChainShape> m_chain;
	std::vector<b2Body*> m_contours;
	b2Body *slopeBody;
	float slopeAngel = -70.0f;
	b2Vec2 slopePosition = b2Vec2(500.0f, 500.0f);

	b2Body *slopeBody2;
	b2Vec2 slopePosition2 = b2Vec2(500.0f, 500.0f);

	b2Body *platformBody;
	b2Vec2 platformPosition = b2Vec2(50.0f, 500.0f);

	MovingPlatform *m_movingPlatform;

private:
	
	TileLayer m_layer;
	
	const float& m_fdt;
	const float& m_dt;

	float m_speed = 100.0f;
	float theta = 0.0f;
	float velX = 1.0f * m_speed;
	bool right = true;
};