#include "GameMap.h"
#include "IsoMap.h"
#include "IsoObject.h"

GameMap::GameMap(IsoMap * isoMap) {
	mIsoMap = isoMap;
}

GameMap::~GameMap() {

}

void GameMap::CreateObjectFromFile(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols) {
	if (objId >= 1000 && objId <= 2000) {
		CreateObject(layerId, 03, r0, c0, rows, cols);
	}
}

void GameMap::CreateObject(unsigned int layerId, unsigned int objId, unsigned int r0, unsigned int c0, unsigned int rows, unsigned int cols) {
	// object origin is out of map
	if (r0 >= m_rows || c0 >= m_cols)
		return;

	// full size is out of map
	const unsigned int r1 = 1 + r0 - rows;
	const unsigned int c1 = 1 + c0 - cols;

	if (r1 >= m_rows || c1 >= m_cols)
		return;

	const unsigned int ind0 = r0 * m_cols + c0;
	mIsoMap->GetLayer(layerId)->AddObject(new IsoObject(rows, cols), r0, c0);
}

