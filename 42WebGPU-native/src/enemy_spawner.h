#pragma once
#include <vector>
#include <engine/Vector.h>
#include <engine/animation/AnimatedModel.h>
#include <Entities/Enemy.h>
#include "enemyOld.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class EnemySpawner {
public:
	EnemySpawner(float _monsterY, std::vector<Enemy*>& _enemies, const AnimatedModel& _player);

	void update(const Vector3f& pos, float dt);
	SceneNode* scene;
	const AnimatedModel& player;

private:
	void spawnEnemy(const Vector3f& pos);
	float getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos);

	// not owned
	std::vector<Enemy*>& enemies;
	
	float countdown;
	const float monsterY;
};