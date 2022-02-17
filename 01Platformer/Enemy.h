#include "Constants.h"
#include "GameObject.h"
#include "CollisionDistances.h"
#include "Quad.h"
#include "Spritesheet_old.h"
#include "Shader.h"

class Character;

class Enemy : public GameObject {

public:
	Enemy(float initialX, float initialY, float maxX, int type);

	void update(float elapsedTime) override;
	void render() override;

	CollisionDistances characterCollides(Character* character);

private:

	Quad *m_quad;
	Shader *m_shader;
	SpritesheetOld *m_spriteSheet;

	float min, max;
	int enemyType;
	bool forward;
	float m_tileCountX;

	const float xScale = ENEMY_WIDTH / (float)(WIDTH);
	const float xTrans = xScale * 2.0f;
	const float yScale = ENEMY_HEIGHT / (float)(HEIGHT);
	const float yTrans = yScale * 2.0f;
};
