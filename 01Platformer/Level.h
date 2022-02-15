#ifndef __levelH__
#define __levelH__

#include <vector>

#include "Constants.h"
#include "Quad.h"
#include "Spritesheet.h"
#include "Shader.h"


class Level {

public:

	Level();

	void render();
	Shader* getShader() const;

	
private:

	void loadLevel();

	Quad *m_quad;
	Shader *m_shader;
	Spritesheet *m_spriteSheet;
	int levelMatrix[LEVEL_HEIGHT][LEVEL_WIDTH];

	const float xScale = MAP_TILE_WIDTH / (float)(WIDTH);
	const float xTrans = xScale * 2.0f;
	const float yScale = MAP_TILE_HEIGHT / (float)(HEIGHT);
	const float yTrans = yScale * 2.0f;
};
#endif // __levelH__
