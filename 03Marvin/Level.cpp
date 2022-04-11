#include "Level.h"

Level::Level(){
	m_shaderArray = Globals::shaderManager.getAssetPointer("level");
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("base");

	loadFile("Resources/Maps/boing.json");

	for (const Tile& tile : m_layer.tiles) {
		addTile(tile, m_vertices, m_indexBuffer);
	}

	short stride = 5, offset = 3;

	glGenBuffers(1, &m_ibo);
	glGenBuffers(1, &m_vbo);
	glGenBuffers(1, &m_vboMap);

	glGenVertexArrays(1, &m_vao);
	glBindVertexArray(m_vao);
	glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
	glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), &m_vertices[0], GL_STATIC_DRAW);

	//positions
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)0);

	//texcoords
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, stride * sizeof(float), (void*)(offset * sizeof(float)));

	//map indices
	glBindBuffer(GL_ARRAY_BUFFER, m_vboMap);
	glBufferData(GL_ARRAY_BUFFER, m_map.size() * sizeof(m_map[0]), &m_map[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Level::loadFile(const std::string &_file) {
	nlohmann::json model;
	std::ifstream file("Resources/Maps/boing2.json");
	file >> model;
	file.close();
	loadLayers(model);
}

void Level::loadLayers(nlohmann::json &map) {

	nlohmann::json layers = map["layers"];
	for (auto &layer : layers) {
		std::string type = layer["type"].get<std::string>();
		if (type.compare("tilelayer") == 0 && (layer["name"].get<std::string>().compare("Platforms") == 0))
			loadTileLayer(layer, m_layer);
	}
}

void Level::loadTileLayer(nlohmann::json &layer_in, TileLayer &layer_out) {
	layer_out.name = layer_in["name"].get<std::string>();
	layer_out.visible = layer_in["visible"].get<bool>();
	layer_out.opacity = layer_in["opacity"].get<float>();
	layer_out.width = layer_in["width"].get<unsigned int>();
	layer_out.height = layer_in["height"].get<unsigned int>();

	nlohmann::json data = layer_in["data"];

	unsigned short count = 0;
	unsigned short posX = 0;
	unsigned short posY = layer_out.height - 1;

	for (auto &tile : data) {
		if (count % layer_out.width == 0 && count > 0) {
			posY--;
			posX = 0;
		}
		if (tile.get<unsigned int>() != 0) {
			layer_out.tiles.push_back({ Vector2f(30.0f * posX, 30.0f * posY), Vector2f(30.0f, 30.0f), tile.get<unsigned int>() - 1});
		}
		posX++;
		count++;
	}
}

void Level::addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices) {
	Vector2f pos = tile.position;
	float w = tile.size[0];
	float h = tile.size[1];

	vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
	vertices.push_back(pos[0]); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

	m_map.push_back(tile.layer); m_map.push_back(tile.layer); m_map.push_back(tile.layer); m_map.push_back(tile.layer);

	unsigned int currentOffset = (vertices.size() / 5) - 4;

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 1);
	indices.push_back(currentOffset + 2);

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 2);
	indices.push_back(currentOffset + 3);
}

void Level::render() {
	glUseProgram(m_shaderArray->m_program);
	m_shaderArray->loadMatrix("u_transform", Globals::projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_spriteSheet->getAtlas());
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}