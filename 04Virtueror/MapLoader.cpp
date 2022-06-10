#include "MapLoader.h"
#include "IsoObject.h"
#include "GameMap.h"

MapLoader::MapLoader() {

}

void MapLoader::setMaps(GameMap * gm, IsoMap * im) {
	mGameMap = gm;
	mIsoMap = im;
}

bool MapLoader::loadLevel(const std::string & filename) {
	mIsoMap->CreateLayer(3);

	std::fstream fs(filename);
	if (!fs.is_open())
		return false;

	readBaseData(fs);
	readObjectsData(fs);
	fs.close();
}

void MapLoader::readBaseData(std::fstream & fs) {
	std::string line;
	std::istringstream ss;

	// reading map size
	std::getline(fs, line);
	ss.str(line);

	unsigned int rows = 0;
	unsigned int cols = 0;
	ss >> rows >> cols;

	mGameMap->m_rows = rows;
	mGameMap->m_cols = cols;

	const int mapSize = rows * cols;
	m_cols = cols;
	m_rows = rows;

	mMap.reserve(mapSize);
	mMap.assign(mapSize, 0);

	mIsoMap->tiles.reserve(mapSize);
	mIsoMap->tiles.assign(mapSize, { Vector2f(),Vector2f(), 0 });

	float pointX;
	float pointY;

	float pointXTrans;
	float pointYTrans;

	// READ BASE MAP
	for (int r = 0; r < rows; ++r) {
		std::getline(fs, line);
		ss.clear();
		ss.str(line);

		//const unsigned int row = r;
		const unsigned int row = rows - 1 - r;
		const unsigned int ind0 = row * cols;

		for (int c = cols - 1; c >= 0; --c) {
			//for (int c = 0; c < cols; ++c) {
			unsigned int type;

			ss >> type;

			const unsigned int index = ind0 + c;
			//mIsoMap->SetCellType(index, type);


			mMap[index] = type;

			pointX = c * (float)(TILE_WIDTH) * 0.5f;
			pointY = row * (float)(TILE_WIDTH) * 0.5f;
			pointXTrans = (pointX - pointY);
			pointYTrans = (pointX + pointY) * 0.5f;

			//tiles[index] = { Vector2f(pointXTrans + 752, pointYTrans - 510),Vector2f(TILE_WIDTH, TILE_HEIGHT), type };


			mIsoMap->tiles[index] = { Vector2f(pointXTrans, pointYTrans),Vector2f(TILE_WIDTH, TILE_HEIGHT), type };

		}
	}
}

bool MapLoader::readObjectsData(std::fstream & fs) {
	std::string line;
	std::istringstream ss;

	// READ OBJECTS
	while (std::getline(fs, line)) {
		// skip comments
		if (!line.empty() && '#' == line[0])
			continue;

		ss.clear();
		ss.str(line);

		unsigned int layerId;
		unsigned int objId;
		unsigned int r0;
		unsigned int c0;
		unsigned int rows;
		unsigned int cols;

		ss >> layerId >> objId >> r0 >> c0 >> rows >> cols;


		mGameMap->CreateObjectFromFile(layerId, objId, r0, c0, rows, cols);
	}

	fs.close();
}

void MapLoader::addTile(const Tile tile, std::vector<float>& vertices, std::vector<unsigned int>& indices, std::vector<unsigned int>& mapIndices) {
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


Vector2f MapLoader::GetCellPosition(unsigned int index) const {

	if (index < mIsoMap->tiles.size())
		return mIsoMap->tiles[index].position;
	else {
		const Vector2f p(-1, -1);
		return p;
	}
}

void MapLoader::setOrigin(const float x, const float y) {
	m_origin[0] = x - TILE_HEIGHT;
	m_origin[1] = y;
	updateTilePositions();
}

void MapLoader::setOrigin(const Vector2f &origin) {
	m_origin = origin;
	m_origin[0] = origin[0] - TILE_HEIGHT;
	updateTilePositions();
}

void MapLoader::updateTilePositions() {

	mIsoMap->m_vertices.clear();
	mIsoMap->m_indexBuffer.clear();
	mIsoMap->m_indexMap.clear();

	for (int i = 0; i < mIsoMap->tiles.size(); i++) {
		mIsoMap->tiles[i].position[0] += m_origin[0];
		mIsoMap->tiles[i].position[1] += m_origin[1];
		addTile(mIsoMap->tiles[i], mIsoMap->m_vertices, mIsoMap->m_indexBuffer, mIsoMap->m_indexMap);


	}
}
