#ifndef __characterH__
#define __characterH__
#include <vector>

#include "Constants.h"
#include "Quad.h"
#include "Spritesheet_old.h"
#include "Shader.h"
#include "Vector.h"




class Character {
public:

	Character();

	void draw();
	void StopFalling(double collisionSize);
	void Jump(bool fullJump);
	void BounceTop();
	void StopMovingLeft(double collisionSize);
	void StopMovingRight(double collisionSize);
	void Die();
	bool IsDead();
	void Reset();
	void render();
	Shader* getShader() const;

	void update(float elapsedTime);
	bool GoesLeft;
	bool GoesRight;

	Matrix4f transform;
	Matrix4f frame;

private:

	Quad *m_quad;
	Shader *m_shader;
	SpritesheetOld *m_spriteSheet;
	
	Vector2f speed;
	Vector2f prevPosition;
	Vector2f position;

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