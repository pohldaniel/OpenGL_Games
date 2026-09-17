#include <cstdlib>

#include "enemy_spawner.h"

namespace {
    const float enemySpawnInterval = 1.0f;
    const int spawnsPerInterval = 1;
    const float spawnRadius = 10.0f;
}

EnemySpawner::EnemySpawner(float _monsterY, const AnimatedModel& _player) : countdown(spawnsPerInterval), monsterY(_monsterY), player(_player) {

}

void EnemySpawner::update(const Vector3f& pos, float dt) {
    countdown -= dt;
    if (countdown <= 0.0f) {
        for (int i = 0; i < spawnsPerInterval; ++i) {
            spawnEnemy(pos);
        }
        countdown += enemySpawnInterval;
    }
}

void EnemySpawner::spawnEnemy(const Vector3f& pos) {
    if (scene->getChildren<Enemy>().size() > 20u)
        return;

    const float theta = glm::radians((float)(rand() % 360));
    const float x = pos[0] + sin(theta) * spawnRadius;
    const float z = pos[2] + cos(theta) * spawnRadius;

    Vector3f spawnPos(x, monsterY, z);
    Quaternion rot;
    rot.rotate(0.0f, getLookAtYRotation(pos, spawnPos), 0.0f);
  
    btCollisionObject* body = Physics::AddKinematicObject(Physics::BtTransform(spawnPos, rot), new btCapsuleShapeZ(0.08f, 0.4f), Physics::collisiontypes::ENEMY, Physics::collisiontypes::SPHERE | Physics::collisiontypes::CHARACTER);
    Enemy* enemy = scene->addChild<Enemy>(body, static_cast<const AnimatedMesh*>(player.getMesh())->getBone(0u).getPosition());
    enemy->setPosition(spawnPos);
    enemy->setOrientation(rot);   
}

float EnemySpawner::getLookAtYRotation(const Vector3f& objectPos, const Vector3f& targetPos) {
    float dx = targetPos[0] - objectPos[0];
    float dz = targetPos[2] - objectPos[2];

    if (abs(dx) < 0.01f && abs(dz) < 0.01f)
        return 0.0f;

    return std::atan2(dx, dz) * _180_ON_PI;
}