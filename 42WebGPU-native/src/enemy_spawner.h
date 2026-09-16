#pragma once
#include <vector>

#include "enemyOld.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class EnemySpawner {
public:
	EnemySpawner(float _monsterY, std::vector<EnemyOld>* _enemies);

	void update(const glm::vec3& playerPos, float deltaTimeSeconds);

private:
	void spawnEnemy(const glm::vec3& playerPos);

	// not owned
	std::vector<EnemyOld>* enemies;
	float countdown;
	const float monsterY;
};