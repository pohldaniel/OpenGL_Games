#pragma once
#include "nlohmann\json.hpp"

#include <iostream>
#include "engine\Vector.h"
#include "engine\Shader.h"
#include "engine\Spritesheet.h"

#include "Constants.h"

struct Tile {
	Vector2f position;
	Vector2f size;
	unsigned int layer;
};

struct TileLayer {

public:
	std::vector<Tile> tiles;
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
	void loadTileLayer(nlohmann::json &layer_in, TileLayer &layer_out);
	void addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices);
	void render();

private:
	Shader *m_shaderArray;
	Spritesheet *m_spriteSheet;
	TileLayer m_layer;

	std::vector<float> m_vertices;
	std::vector<unsigned int> m_indexBuffer;
	std::vector<unsigned int> m_map;
	unsigned int m_vao = 0;
	unsigned int m_vbo = 0;
	unsigned int m_vboMap = 0;
	unsigned int m_ibo = 0;
};