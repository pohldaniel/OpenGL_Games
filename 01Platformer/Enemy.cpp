#include "Enemy.h"
#include "Character.h"

Enemy::Enemy(float initialX, float initialY, float maxX, int type){
	m_shader = new Shader("shader/quad_trans.vs", "shader/quad_trans.fs");
	m_spriteSheet = new SpritesheetOld("enemies.png", ENEMY_TILE_WIDTH, ENEMY_TILE_HEIGHT, true, true);
	m_quad = new Quad(xScale, yScale);

	m_position[0] = initialX;
	m_position[1] = initialY;

	min = initialX;
	max = maxX;

	enemyType = type;
	forward = true;

	m_tileCountX = m_spriteSheet->getTileCountX();
}


void Enemy::update(float elapsedTime) {
	// moves the enemy and turns it around if it gets to the limits
	m_position[0] += (forward ? 25 : -25) * elapsedTime;

	if (m_position[0] >= max){
		forward = false;
	}

	if (m_position[0] <= min){
		forward = true;
	}
}

CollisionDistances Enemy::characterCollides(Character* character){ 
	// returns collision details if the enemy collides with the character

	double charTop = character->getPosition()[1] - CHARACTER_HEIGHT;
	double charBottom = character->getPosition()[1];
	double charLeft = character->getPosition()[0] - CHARACTER_WIDTH / 2 + 3;
	double charRight = character->getPosition()[0] + CHARACTER_WIDTH / 2 - 4;

	double enemyTop = m_position[1] - ENEMY_HEIGHT;
	double enemyBottom = m_position[1];
	double enemyLeft = m_position[0] - ENEMY_WIDTH / 2 + 3;
	double enemyRight = m_position[0] + ENEMY_WIDTH / 2 - 4;

	CollisionDistances collision;
	collision.top = 0;
	collision.bottom = 0;
	collision.left = 0;
	collision.right = 0;

	// If it's a collision
	if (charTop < enemyBottom && charBottom > enemyTop&& charRight > enemyLeft&& charLeft < enemyRight){
		collision.top = abs(charTop - enemyBottom);
		collision.bottom = abs(charBottom - enemyTop);
		collision.left = abs(charLeft - enemyRight);
		collision.right = abs(charRight - enemyLeft);

		collision.keepSmallest();
	}

	return collision;
}

void Enemy::render() {

	int step = enemyType * m_tileCountX + (int)m_position[0] % 20 / 10 + (forward ? 2 : 0);

	glUseProgram(m_shader->m_program);
	m_shader->loadMatrix("u_transform", Matrix4f::Translate(m_transform, ((m_position[0] - Globals::offset) / ENEMY_TILE_WIDTH) * xTrans - 1.0f, -1.0f + yTrans * ((HEIGHT - m_position[1]) / ENEMY_TILE_HEIGHT + 0.5f), 0.0f));
	m_shader->loadMatrix("u_frame", m_spriteSheet->getFrameTransform(step));

	m_quad->render(m_spriteSheet->getTexture());
	glUseProgram(0);
}