#pragma once

#include <vector>

#include <engine/Vector.h>
#include "ThreadPool.h"

class Enemy;
class BulletStore {
public:
 

  void createBullets(const Vector3f& position, const Quaternion& midOri, const int spreadAmount);
  void updateBullets(float deltaTimeSeconds, std::vector<Enemy*>& enemies);

  std::vector<Vector4f> m_offsets;
  std::vector<Quaternion> m_rots;
  std::vector<Vector3f> allBulletDirs;

  struct BulletGroup {
    int startIndex;
    int groupSize;
    float TTL;

    BulletGroup(int _startIndex, int _groupSize, float lifetime)
    {
      TTL = lifetime;
      startIndex = _startIndex;
      groupSize = _groupSize;
    }
  };
  BulletStore(ThreadPool* const _threadPool)
    : threadPool(_threadPool) {}

  ThreadPool* const threadPool;
  std::vector<BulletGroup> bulletGroups;
};
