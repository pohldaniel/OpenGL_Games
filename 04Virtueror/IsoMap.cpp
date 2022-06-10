#include "IsoMap.h"

IsoMap::IsoMap() {
	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("tiles");
	m_shaderLevel = Globals::shaderManager.getAssetPointer("level");
}

IsoMap::~IsoMap() {}

void IsoMap::render(Matrix4f& transform) {
	glUseProgram(m_shaderLevel->m_program);
	m_shaderLevel->loadMatrix("u_transform", transform * Globals::projection);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, m_spriteSheet->getAtlas());
	glBindVertexArray(m_vao);
	glDrawElements(GL_TRIANGLES, m_indexBuffer.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
	glUseProgram(0);
}

void IsoMap::addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices) {
	Vector2f pos = tile.position;
	float w = tile.size[0];
	float h = tile.size[1];

	vertices.push_back(pos[0]); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(0.0f);
	vertices.push_back(pos[0]); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(0.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1] + h); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(1.0f);
	vertices.push_back(pos[0] + w); vertices.push_back(pos[1]); vertices.push_back(0.0f); vertices.push_back(1.0f); vertices.push_back(0.0f);

	mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid); mapIndices.push_back(tile.gid);

	unsigned int currentOffset = (vertices.size() / 5) - 4;

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 1);
	indices.push_back(currentOffset + 2);

	indices.push_back(currentOffset + 0);
	indices.push_back(currentOffset + 2);
	indices.push_back(currentOffset + 3);
}

IsoLayer * IsoMap::CreateLayer(unsigned int layerId) {
	// check layer has not been created yet
	IsoLayer * layer = GetLayer(layerId);

	if (layer != nullptr)
		return layer;

	// create and store new layer
	layer = new IsoLayer();

	mLayers.emplace_back(layer);
	mLayersMap.insert({ layerId, layer });
	mLayersRenderList.emplace_back(layer);

	return layer;
}

void IsoMap::setSize(unsigned int rows, unsigned int cols) {

	if (rows == m_rows && cols == m_cols)
		return;

	m_rows = rows;
	m_cols = cols;

	const int mapSize = rows * cols;

	tiles.reserve(mapSize);
	tiles.assign(mapSize, { Vector2f(),Vector2f(), 0 });

	// update layers
	//for (IsoLayer * layer : mLayers)
	//layer->UpdateSize();

	// update tiles
	updateTilePositions();
}

void IsoMap::setOrigin(const float x, const float y) {
	m_origin[0] = x - TILE_HEIGHT;
	m_origin[1] = y;
	updateTilePositions();
}

void IsoMap::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_origin[0] = origin[0] - TILE_HEIGHT;
	updateTilePositions();
}

void IsoMap::updateTilePositions() {

	m_vertices.clear();
	m_indexBuffer.clear();
	m_indexMap.clear();

	for (int i = 0; i < tiles.size(); i++) {
		tiles[i].position[0] += m_origin[0];
		tiles[i].position[1] += m_origin[1];
		addTile(tiles[i], m_vertices, m_indexBuffer, m_indexMap);
	}

	updateBuffer();
}

void IsoMap::updateBuffer() {
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
	glBufferData(GL_ARRAY_BUFFER, m_indexMap.size() * sizeof(m_indexMap[0]), &m_indexMap[0], GL_STATIC_DRAW);
	glEnableVertexAttribArray(2);
	glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 1 * sizeof(unsigned int), 0);

	//indices
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ibo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer.size() * sizeof(unsigned int), &m_indexBuffer[0], GL_STATIC_DRAW);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

Vector2f IsoMap::GetCellPosition(unsigned int index) const {

	if (index < tiles.size())
		return tiles[index].position;
	else {
		const Vector2f p(-1, -1);
		return p;
	}
}