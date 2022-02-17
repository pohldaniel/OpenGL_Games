#ifndef __levelH__
#define __levelH__

#include <vector>

#include "Constants.h"
#include "GameObject.h"
#include "CollisionDistances.h"
#include "Quad.h"
#include "Spritesheet.h"
#include "Shader.h"

class Character;

class Level : public GameObject {

public:

	Level();

	void update(float elapsedTime) override {}
	void render() override;
	
	CollisionDistances characterCollides(Character *character);
	int pickUpCollectibles(Character* character);
	bool levelExit(Character* character);

private:

	void loadLevel();

	Quad *m_quad;
	Shader *m_shader;
	Spritesheet *m_spriteSheet;
	int levelMatrix[LEVEL_HEIGHT][LEVEL_WIDTH];
	int levelSolids[LEVEL_HEIGHT][LEVEL_WIDTH];
	int levelCollectible[LEVEL_HEIGHT][LEVEL_WIDTH];
	bool tileIsSolid(int tileNo);
	bool tileIsCollectible(int tileNo);

	int solids[34] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 13, 14, 15, 16, 17, 18, 19, 20, 24, 25, 26, 27, 31, 32, 33, 34, 41, 42, 43, 44, 49, 50 };
	int collectibles[8] = { 54, 55, 56, 57, 61, 62, 63, 64 };

	const float xScale = MAP_TILE_WIDTH / (float)(WIDTH);
	const float xTrans = xScale * 2.0f;
	const float yScale = MAP_TILE_HEIGHT / (float)(HEIGHT);
	const float yTrans = yScale * 2.0f;
};
#endif // __levelH__
