#include <engine/Batchrenderer.h>
#include <engine/Spritesheet.h>
#include "TileSet.h"

#include "Level.h"

Level::Level(const Camera& _camera, int setTileCountX, int setTileCountY) : camera(_camera), tileCountX(setTileCountX), tileCountY(setTileCountY), targetX(setTileCountX / 2), targetY(setTileCountY / 2) {
	size_t listTilesSize = (size_t)tileCountX * tileCountY;
	m_tiles.assign(listTilesSize, Tile());

	//Add an enemy spawner at each corner.
	int xMax = tileCountX - 1;
	int yMax = tileCountY - 1;
	setTileType(0, 0, TileType::ENEMY_SPAWNER);
	setTileType(xMax, 0, TileType::ENEMY_SPAWNER);
	setTileType(0, yMax, TileType::ENEMY_SPAWNER);
	setTileType(xMax, yMax, TileType::ENEMY_SPAWNER);

	calculateFlowField();
}

Level::~Level() {

}

void Level::init(std::vector<TextureRect>& textureRects) {
	m_textureRects = textureRects;
}

void Level::draw(float tileSize) {

	int count = 0;

	for (auto& tile : m_tiles) {
		int x = (count % tileCountX);
		int y = (count / tileCountX);
		Tile& tileSelected = m_tiles[x + y * tileCountX];
		//if (tileSelected.type == TileType::EMPTY) {
			/*int index = 1;
			if (tileSelected.flowDirectionX == 0 && tileSelected.flowDirectionY == -1) {
				index = 2;
			}else if (tileSelected.flowDirectionX == 1 && tileSelected.flowDirectionY == -1) {
				index = 3;
			}else if (tileSelected.flowDirectionX == 1 && tileSelected.flowDirectionY == 0) {
				index = 4;
			}else if (tileSelected.flowDirectionX == 1 && tileSelected.flowDirectionY == 1) {
				index = 5;
			}else if (tileSelected.flowDirectionX == 0 && tileSelected.flowDirectionY == 1) {
				index = 6;
			}else if (tileSelected.flowDirectionX == -1 && tileSelected.flowDirectionY == 1) {
				index = 7;
			}else if (tileSelected.flowDirectionX == -1 && tileSelected.flowDirectionY == 0) {
				index = 8;
			}else if (tileSelected.flowDirectionX == -1 && tileSelected.flowDirectionY == -1) {
				index = 9;
			}

			const TextureRect& rect = m_rextureRects[index];
			Batchrenderer::Get().addQuadAA(Vector4f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize), tileSize, tileSize), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
			*/
			const TextureRect& rect = m_textureRects[12];
			Batchrenderer::Get().addQuadAA(Vector4f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize), tileSize, tileSize), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), (x + y) % 2 == 0 ? Vector4f(0.94118f, 0.94118f, 0.94118f, 1.0f) : Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
			
		//}
		count++;
	}

	count = 0;
	for (auto& tile : m_tiles) {
		
		int x = (count % tileCountX);
		int y = (tileCountY - 1) - (count / tileCountX);
		Tile& tileSelected = m_tiles[x + y * tileCountX];

		if (tileSelected.type == TileType::EMPTY) {
			
			if ((x == targetX && y == targetY)) {
				const TextureRect& target = m_textureRects[10];
				Batchrenderer::Get().addQuadAA(Vector4f(static_cast<float>(targetX * tileSize), static_cast<float>(targetY * tileSize), tileSize, tileSize), Vector4f(target.textureOffsetX, target.textureOffsetY, target.textureWidth, target.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), target.frame);
			}/*else {
				int index = 1;
				if (tileSelected.flowDirectionX == 0 && tileSelected.flowDirectionY == -1) {
					index = 2;
				}
				else if (tileSelected.flowDirectionX == 1 && tileSelected.flowDirectionY == -1) {
					index = 3;
				}
				else if (tileSelected.flowDirectionX == 1 && tileSelected.flowDirectionY == 0) {
					index = 4;
				}
				else if (tileSelected.flowDirectionX == 1 && tileSelected.flowDirectionY == 1) {
					index = 5;
				}
				else if (tileSelected.flowDirectionX == 0 && tileSelected.flowDirectionY == 1) {
					index = 6;
				}
				else if (tileSelected.flowDirectionX == -1 && tileSelected.flowDirectionY == 1) {
					index = 7;
				}
				else if (tileSelected.flowDirectionX == -1 && tileSelected.flowDirectionY == 0) {
					index = 8;
				}
				else if (tileSelected.flowDirectionX == -1 && tileSelected.flowDirectionY == -1) {
					index = 9;
				}

				const TextureRect& rect = m_rextureRects[index];
				Batchrenderer::Get().addQuadAA(Vector4f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize), tileSize, tileSize), Vector4f(rect.textureOffsetX, rect.textureOffsetY, rect.textureWidth, rect.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), rect.frame);
			}*/

		}else if (tileSelected.type == TileType::ENEMY_SPAWNER) {
			const TextureRect& wall = m_textureRects[11];
			Batchrenderer::Get().addQuadAA(Vector4f(static_cast<float>(x * tileSize), static_cast<float>(y * tileSize), tileSize, tileSize), Vector4f(wall.textureOffsetX, wall.textureOffsetY, wall.textureWidth, wall.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), wall.frame);
		}else if (tileSelected.type == TileType::WALL) {
			const TextureRect& wall = m_textureRects[0];
			Batchrenderer::Get().addQuadAA(Vector4f(static_cast<float>(x * tileSize + tileSize / 2 ) - static_cast<float>(wall.width / 2), static_cast<float>(y * tileSize + tileSize / 2) - static_cast<float>(wall.height / 2), static_cast<float>(wall.width), static_cast<float>(wall.height)), Vector4f(wall.textureOffsetX, wall.textureOffsetY, wall.textureWidth, wall.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), wall.frame);
			
			//Batchrenderer::Get().addQuadAA(Vector4f(static_cast<float>(x * tileSize), static_cast<float>(y* tileSize), tileSize, tileSize), Vector4f(wall.textureOffsetX, wall.textureOffsetY, wall.textureWidth, wall.textureHeight), Vector4f(1.0f, 1.0f, 1.0f, 1.0f), wall.frame);
		}
		count++;
	}
	

	Batchrenderer::Get().drawBuffer();

}

TileType Level::getTileType(int x, int y) {
	int index = x + y * tileCountX;
	if (index > -1 && index < m_tiles.size() &&
		x > -1 && x < tileCountX &&
		y > -1 && y < tileCountY)
		return m_tiles[index].type;

	return TileType::EMPTY;
}

void Level::setTileType(int x, int y, TileType tileType) {
	int index = x + y * tileCountX;
	if (index > -1 && index < m_tiles.size() &&
		x > -1 && x < tileCountX &&
		y > -1 && y < tileCountY) {
		m_tiles[index].type = tileType;
		calculateFlowField();
	}
}

bool Level::isTileWall(int x, int y) {
	return (getTileType(x, y) == TileType::WALL);
}


void Level::setTileWall(int x, int y, bool setWall) {
	if (getTileType(x, y) != TileType::ENEMY_SPAWNER)
		setTileType(x, y, (setWall ? TileType::WALL : TileType::EMPTY));
}

Vector2f Level::getTargetPos() {
	return Vector2f((float)targetX + 0.5f, (float)targetY + 0.5f);
}

void Level::calculateFlowField() {
	//Ensure the target is in bounds.
	int indexTarget = targetX + targetY * tileCountX;

	if (indexTarget > -1 && indexTarget < m_tiles.size() &&
		targetX > -1 && targetX < tileCountX &&
		targetY > -1 && targetY < tileCountY) {
			
		//Reset the tile flow data.
		for (auto& tileSelected : m_tiles) {
			tileSelected.flowDirectionX = 0;
			tileSelected.flowDirectionY = 0;
			tileSelected.flowDistance = flowDistanceMax;
		}

		//Calculate the flow field.
		calculateDistances();
		calculateFlowDirections();
	}	
}

void Level::calculateDistances() {
	int indexTarget = targetX + targetY * tileCountX;

	//Create a queue that will contain the indices to be checked.
	std::queue<int> listIndicesToCheck;
	//Set the target tile's flow value to 0 and add it to the queue.
	m_tiles[indexTarget].flowDistance = 0;
	listIndicesToCheck.push(indexTarget);

	//The offset of the neighboring tiles to be checked.
	const int listNeighbors[][2] = { { -1, 0 },{ 1, 0 },{ 0, -1 },{ 0, 1 } };

	//Loop through the queue and assign distance to each tile.
	while (listIndicesToCheck.empty() == false) {
		int indexCurrent = listIndicesToCheck.front();
		listIndicesToCheck.pop();

		//Check each of the neighbors;
		for (int count = 0; count < 4; count++) {
			int neighborX = listNeighbors[count][0] + indexCurrent % tileCountX;
			int neighborY = listNeighbors[count][1] + indexCurrent / tileCountX;
			int indexNeighbor = neighborX + neighborY * tileCountX;

			//Ensure that the neighbor exists and isn't a wall.
			if (indexNeighbor > -1 && indexNeighbor < m_tiles.size() &&
				neighborX > -1 && neighborX < tileCountX &&
				neighborY > -1 && neighborY < tileCountY &&
				m_tiles[indexNeighbor].type != TileType::WALL) {

				//Check if the tile has been assigned a distance yet or not.
				if (m_tiles[indexNeighbor].flowDistance == flowDistanceMax) {
					//If not the set it's distance and add it to the queue.
					m_tiles[indexNeighbor].flowDistance = m_tiles[indexCurrent].flowDistance + 1;
					listIndicesToCheck.push(indexNeighbor);
				}
			}
		}
	}
}


void Level::calculateFlowDirections() {
	//The offset of the neighboring tiles to be checked.
	const int listNeighbors[][2] = {
		{ -1, 0 },{ -1, 1 },{ 0, 1 },{ 1, 1 },
		{ 1, 0 },{ 1, -1 },{ 0, -1 },{ -1, -1 } };

	for (int indexCurrent = 0; indexCurrent < m_tiles.size(); indexCurrent++) {
		//Ensure that the tile has been assigned a distance value.
		if (m_tiles[indexCurrent].flowDistance != flowDistanceMax) {
			//Set the best distance to the current tile's distance.
			unsigned char flowFieldBest = m_tiles[indexCurrent].flowDistance;

			//Check each of the neighbors;
			for (int count = 0; count < 8; count++) {
				int offsetX = listNeighbors[count][0];
				int offsetY = listNeighbors[count][1];

				int neighborX = offsetX + indexCurrent % tileCountX;
				int neighborY = offsetY + indexCurrent / tileCountX;
				int indexNeighbor = neighborX + neighborY * tileCountX;

				//Ensure that the neighbor exists.
				if (indexNeighbor > -1 && indexNeighbor < m_tiles.size() &&
					neighborX > -1 && neighborX < tileCountX &&
					neighborY > -1 && neighborY < tileCountY) {
					//If the current neighbor's distance is lower than the best then use it.
					if (m_tiles[indexNeighbor].flowDistance < flowFieldBest) {
						flowFieldBest = m_tiles[indexNeighbor].flowDistance;
						m_tiles[indexCurrent].flowDirectionX = offsetX;
						m_tiles[indexCurrent].flowDirectionY = offsetY;
					}
				}
			}
		}
	}
}

Vector2f Level::getFlowNormal(int x, int y) {
	int index = x + y * tileCountX;
	if (index > -1 && index < m_tiles.size() &&
		x > -1 && x < tileCountX &&
		y > -1 && y < tileCountY)
		return Vector2f((float)m_tiles[index].flowDirectionX, (float)m_tiles[index].flowDirectionY).normalize();

	return Vector2f();
}

Vector2f Level::getRandomEnemySpawnerLocation() {
	//Create a list of all tiles that are enemy spawners.
	std::vector<int> listSpawnerIndices;
	for (int count = 0; count < m_tiles.size(); count++) {
		auto& tileSelected = m_tiles[count];
		if (tileSelected.type == TileType::ENEMY_SPAWNER)
			listSpawnerIndices.push_back(count);
	}

	//If one or more spawners are found, pick one at random and output it's center position.
	if (listSpawnerIndices.empty() == false) {
		int index = listSpawnerIndices[rand() % listSpawnerIndices.size()];
		return Vector2f((float)(index % tileCountX) + 0.5f, (float)(index / tileCountX) + 0.5f);
	}

	return Vector2f(0.5f, 0.5f);
}