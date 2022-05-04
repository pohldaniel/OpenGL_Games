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

	//b2World* getWorld() const;

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

	const float& m_fdt;
	const float& m_dt;

	float m_speed = 100.0f;
	float theta = 0.0f;
	float velX = 1.0f * m_speed;
	bool right = true;

	float m_speed2 = 100.0f;
	float velY = 1.0f * m_speed2;
	bool upward = true;

	bool tmp = false;


	b2Vec2 initialPosition_ = b2Vec2(700.0f, 500.0f);
	b2Vec2 finishPosition_ = b2Vec2(700.0f, 700.0f);
	b2Vec2 directionToFinish_;
	float maxLiftSpeed_ = 1.0f;
	float minLiftSpeed_ = 0.1f;
	float curLiftSpeed_ = 1.0f;

	int platformState_ = PLATFORM_STATE_MOVETO_FINISH;
	enum PlatformStateType
	{
		PLATFORM_STATE_START,
		PLATFORM_STATE_MOVETO_FINISH,
		PLATFORM_STATE_MOVETO_START,
		PLATFORM_STATE_FINISH
	};

	inline float Clamp(float value, float min, float max)
	{
		if (value < min)
			return min;
		else if (value > max)
			return max;
		else
			return value;
	}
};