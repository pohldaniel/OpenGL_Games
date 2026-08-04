#ifndef _SD_ANG_BULLET_STORE_H_
#define _SD_ANG_BULLET_STORE_H_

#include <vector>

#include <glm/glm.hpp>
#include "ThreadPool.h"

class BulletStore {
public:
 

  void createBullets(const glm::vec3& position, const glm::quat& midOri, const int spreadAmount);
  void updateBullets(float deltaTimeSeconds);

  std::vector<glm::vec4> m_offsets;
  std::vector<glm::quat> m_rots;
  std::vector<glm::vec3> allBulletDirs;

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

#endif // _SD_ANG_BULLET_STORE_H_
