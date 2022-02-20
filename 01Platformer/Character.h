#ifndef __characterH__
#define __characterH__
#include <vector>

#include "Constants.h"
#include "GameObject.h"
#include "Quad.h"
#include "Spritesheet_old.h"
#include "Shader.h"


class Character : public GameObject{
public:

	Character();
	~Character();

	void update(float elapsedTime) override;
	void render() override;

	void stopFalling(double collisionSize);
	void jump(bool fullJump);
	void bounceTop();
	void stopMovingLeft(double collisionSize);
	void stopMovingRight(double collisionSize);
	bool isDead();
	void die();	
	void reset();
	
	bool goesLeft;
	bool goesRight;

private:

	Quad *m_quad;
	Shader *m_shader;
	SpritesheetOld *m_spriteSheet;

	Vector2f m_speed;
	Vector2f m_prevPosition;

	bool forward = true;
	bool dead;

	int tileIndex = 59;
	int step = 1;

	float xScale = CHARACTER_TILE_WIDTH / (float)(WIDTH);
	float xTrans = xScale * 2.0f;
	float yScale = CHARACTER_TILE_HEIGHT / (float)(HEIGHT);
	float yTrans = yScale * 2.0f;
	
};

#endif // __cameraH__