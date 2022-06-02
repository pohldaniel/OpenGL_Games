#include "MapLoader.h"

bool MapLoader::loadLevel(const std::string & filename) {

	m_spriteSheet = Globals::spritesheetManager.getAssetPointer("tiles2");

	std::fstream fs(filename);
	if (!fs.is_open())
		return false;

	std::string line;
	std::istringstream ss;

	// reading map size
	std::getline(fs, line);
	ss.str(line);

	unsigned int rows = 0;
	unsigned int cols = 0;
	ss >> rows >> cols;

	const int mapSize = rows * cols;

	mMap.reserve(mapSize);
	mMap.assign(mapSize, 0);

	mTilePositions.reserve(mapSize);
	mTilePositions.assign(mapSize, Vector2f());

	tiles.reserve(mapSize);
	tiles.assign(mapSize, { Vector2f(),Vector2f(), 0 });

	float pointX;
	float pointY;

	float pointXTrans;
	float pointYTrans;

	// READ BASE MAP
	for (int r = 0; r < rows; ++r){
		std::getline(fs, line);
		ss.clear();
		ss.str(line);

		const unsigned int ind0 = r * cols;

		for (int c = 0; c < cols; ++c){
			unsigned int type;

			ss >> type;

			const unsigned int index = ind0 + c;
			
			//mIsoMap->SetCellType(index, type);

			if (index < mMap.size() && type < m_spriteSheet->getTotalFrames()) {
				mMap[index] = type;
				
				
				pointX = c * (float)(TILE_WIDTH) * 0.5f;
				pointY = r * (float)(TILE_WIDTH) * 0.5f;			
				pointXTrans = (pointX - pointY);
				pointYTrans = (pointX + pointY) * 0.5f;
				tiles[index] = { Vector2f(pointXTrans, pointYTrans),Vector2f(TILE_WIDTH, TILE_HEIGHT), type };

				
			}
		}
	}

	fs.close();

	for (const Tile& tile : tiles) {
		addTile(tile, m_vertices, m_indexBuffer, m_indexMap);

	}
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