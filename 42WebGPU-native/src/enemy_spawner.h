#pragma once
#include <vector>
#include <engine/Vector.h>
#include <engine/animation/AnimatedModel.h>
#include <Entities/Enemy.h>

class EnemySpawner {

public:

	EnemySpawner(float _monsterY, const AnimatedModel& _player);

	void update(const Vector3f& pos, float dt);
	SceneNode* scene;
	size_t& count();
private:

	void spawnEnemy(const Vector3f& pos);
	float getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos);
	const AnimatedModel& player;
	float countdown;
	const float monsterY;
	size_t m_count;
};