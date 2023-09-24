#include "MapLoader.h"
#include "IsoMap.h"
#include "GameMap.h"

MapLoader::MapLoader() {
}

MapLoader::MapLoader(GameMap * gm, IsoMap * im) : mGameMap(gm), mIsoMap(im) {
}

void MapLoader::setMaps(GameMap * gm, IsoMap * im) {
	mGameMap = gm;
	mIsoMap = im;
}

bool MapLoader::loadLevel(const std::string & filename) {
	const int rendW = 1600;
	const int rendH = 900;
	
	mIsoMap->CreateLayer(3);

	std::fstream fs(filename);
	if (!fs.is_open())
		return false;

	readBaseData(fs);

	const int mapH = mIsoMap->GetHeight();
	mIsoMap->setOrigin(rendW * 0.5f - (float)TILE_WIDTH * 0.5f, rendH *  0.5f - (float)TILE_HEIGHT * 0.5f);

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

	mIsoMap->setSize(rows, cols);

	mGameMap->m_rows = rows;
	mGameMap->m_cols = cols;

	float pointX, pointY;
	float pointXTrans, pointYTrans;

	// READ BASE MAP
	for (int r = 0; r < rows; ++r) {
		std::getline(fs, line);
		ss.clear();
		ss.str(line);

		const unsigned int row = r;
		const unsigned int ind0 = row * cols;

		for (int c = 0; c < cols; ++c) {
			unsigned int type;

			ss >> type;

			const unsigned int index = ind0 + c;
			pointX = c * (float)(TILE_WIDTH) * 0.5f;
			pointY = row * (float)(TILE_WIDTH) * 0.5f;
			pointXTrans = (pointX - pointY);
			pointYTrans = (pointX + pointY) * 0.5f;

			mIsoMap->tiles[index] = { Vector2f(pointXTrans, -pointYTrans),Vector2f(TILE_WIDTH, TILE_HEIGHT), type };
			//std::cout << row << "  " << c << "  " << pointXTrans << "  " << -pointYTrans << std::endl;
		}
	}
	
	if (mGameMap){
		mGameMap->SetSize(rows, cols);
		//mGameMap->SyncMapCells();
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

	return true;
}