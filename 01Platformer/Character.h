#ifndef __characterH__
#define __characterH__
#include <vector>

#include "Shader.h"
#include "Vector.h"

#define CHARACTER_WIDTH 50
#define CHARACTER_HEIGHT 66
#define CHARACTER_TILE_HEIGHT 100
#define LEVEL_WIDTH 100
#define LEVEL_HEIGHT 12
#define TILE_SIZE 50

#define WIDTH 800
#define HEIGHT 600
#define RESOLUTION_X 800
#define RESOLUTION_Y 600


#define MAP_TILE_DIM_X 50
#define MAP_TILE_DIM_Y 100
#define TILE_SIZE_X 50
#define TILE_SIZE_Y 100

#define CHARACTER_TILE_HEIGHT 100

class Character 
{
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
	void render2();
	Shader* getShader() const;

	void update(float elapsedTime);
	bool GoesLeft;
	bool GoesRight;

	Matrix4f transform;
	Matrix4f frame;

private:

	const float WIDTH1PX = 2.0f / (float)(WIDTH);
	const float HEIGHT1PX = 2.0f / (float)(HEIGHT);

	Vector2f speed;
	Vector2f prevPosition;
	Vector2f position;

	bool forward = true;
	bool dead;

	void createBuffer();
	void loadTileset();
	Matrix4f calcTileFrameTransform(int id);
	unsigned int m_vao;
	float m_sizeX = 1;
	float m_sizeY = 1;
	float m_sizeTex = 1;
	std::vector<float> m_vertex;

	Shader *m_quadShader;

	int tileCountX, tileCountY;
	double tileScaleX, tileScaleY;
	int totalFrames;
	std::vector<Matrix4f> tileFrameTransforms;
	GLuint m_texture;

	int tileIndex = 59;
	int step = 1;
	
};

#endif // __cameraH__