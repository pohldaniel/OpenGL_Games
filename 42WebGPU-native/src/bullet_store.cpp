#include "bullet_store.h"

#include <chrono>
#include <algorithm>
#include <iostream>
#include <thread>


#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

#define _USE_MATH_DEFINES
#include <math.h>

namespace {


const float pi = (float)M_PI;
const float rotPerBullet = 3.0f * pi / 180.0f;

glm::quat partialHamiltonProduct2(const glm::quat& q1, const glm::vec3& q2) {
  return glm::quat(
    - q1.x * q2.x - q1.y * q2.y - q1.z * q2.z,
    q1.w * q2.x + q1.y * q2.z - q1.z * q2.y,
    q1.w * q2.y - q1.x * q2.z + q1.z * q2.x,
    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x);
}

glm::vec3 partialHamiltonProduct(const glm::quat& q1, const glm::quat& q2) {
  return glm::vec3(
    q1.w * q2.x + q1.x * q2.w + q1.y * q2.z - q1.z * q2.y,
    q1.w * q2.y - q1.x * q2.z + q1.y * q2.w + q1.z * q2.x,
    q1.w * q2.z + q1.x * q2.y - q1.y * q2.x + q1.z * q2.w);
}

glm::vec3 rotateByQuat(const glm::vec3& v, const glm::quat& q) {
  const glm::quat qPrime = glm::quat(q.w, -q.x, -q.y, -q.z);
  return partialHamiltonProduct(partialHamiltonProduct2(q, v), qPrime);
}

const float bulletScale = 0.3f;
const float bulletLifetime = 1.0f; // seconds
const glm::vec3 scaleVec(bulletScale, bulletScale, bulletScale);
const float bulletSpeed = 15.0f; // Game units per second
const glm::vec3 bulletNormal(0.0f, 1.0f, 0.0f);
const glm::vec3 canonicalDir(0.0f, 0.0f, 1.0f);

// TODO double sided?
const float bulletVertices[] = {
    // Positions                                            // Tex Coords
    bulletScale * (-0.243f), 0.0f, bulletScale * (-0.5f),   1.0f, 0.0f,
    bulletScale * (-0.243f), 0.0f, bulletScale * 0.5f,      0.0f, 0.0f,
    bulletScale * 0.243f,  0.0f, bulletScale * 0.5f,        0.0f, 1.0f,
    bulletScale * 0.243f, 0.0f, bulletScale * (-0.5f),      1.0f, 1.0f
};
const unsigned int bulletIndices[] = {
    0, 1, 2,
    0, 2, 3
};

}  // namespace


void BulletStore::createBullets(const glm::vec3& position, const glm::quat& midOri, const int spreadAmount) {
    if (m_rots.size() >= 4000)
        return;

  const int startIndex = m_offsets.size();
  const int bulletGroupSize = spreadAmount * spreadAmount;
  BulletGroup g(startIndex, bulletGroupSize, bulletLifetime);
  m_offsets.resize(startIndex + bulletGroupSize);
  m_rots.resize(startIndex + bulletGroupSize);
  allBulletDirs.resize(startIndex + bulletGroupSize);
  const int parallelism = threadPool->numWorkers();
  const int workerGroupSize = spreadAmount / parallelism;
  std::vector<std::future<void>> futures;
  for (int p = 0; p < parallelism; ++p) {
    const int iStart = p * workerGroupSize;
    const int iEnd = p == (parallelism - 1) ? spreadAmount : iStart + workerGroupSize;
    futures.emplace_back(threadPool->enqueue([this, &position, &midOri, spreadAmount, startIndex, &g, iStart, iEnd]() {
      for (int i = iStart; i < iEnd; ++i) {
        const glm::quat yQuat = glm::rotate(
            midOri,
            rotPerBullet * (i - spreadAmount / 2) - glm::pi<float>() * 0.5f,
            glm::vec3(0.0f, 1.0f, 0.0f));
        for (int j = 0; j < spreadAmount; ++j) {
            const glm::quat rotQuat = glm::rotate(
              yQuat,
              rotPerBullet * (j - spreadAmount / 2),
              glm::vec3(0.0f, 0.0f, 1.0f));
          const glm::vec3 dir = rotateByQuat(glm::vec3(1.0f, 0.0f, 0.0f), rotQuat);
          const int pos = i * spreadAmount + j + startIndex;
          m_offsets[pos] = glm::vec4(position, 0.0f);
          allBulletDirs[pos] = dir;
          m_rots[pos] = rotQuat;
        }
      }
    }));
  }
  for (auto& future : futures) {
    future.get();
  }
  bulletGroups.push_back(g);
}

void BulletStore::updateBullets(float deltaTimeSeconds) {

    // Bullet groups are divided into subgroups, which are excluded en masse from
      // enemy collision detection.
    const bool useAABB = false;
    const int numSubGroups = useAABB ? 9 : 1;

    const float deltaPosMagnitude = deltaTimeSeconds * bulletSpeed;
    int firstLiveBulletGroup = 0;

    // True iff enemies[i] is dead from bullet collision.
    //std::vector<bool> enemyDeathMarker(enemies->size());

    std::vector<std::future<void>> futures;
    for (BulletGroup& g : bulletGroups) {
        g.TTL -= deltaTimeSeconds;
        if (g.TTL <= 0.0f) {
            firstLiveBulletGroup++;
        }
        else {
            futures.emplace_back(threadPool->enqueue([this, numSubGroups, useAABB, deltaPosMagnitude, &g]() {
                const int bulletGroupStartIdx = g.startIndex;
                const int numBulletsInGroup = g.groupSize;
                const int subgroupSize = numBulletsInGroup / numSubGroups;
                for (int subgroup = 0; subgroup < numSubGroups; ++subgroup) {
                    int bulletsStart = subgroupSize * subgroup;
                    int bulletsEnd = (subgroup == (numSubGroups - 1)) ? numBulletsInGroup : (bulletsStart + subgroupSize);
                    bulletsStart += bulletGroupStartIdx;
                    bulletsEnd += bulletGroupStartIdx;

                    for (int bulletIdx = bulletsStart; bulletIdx < bulletsEnd; ++bulletIdx) {
                        glm::vec4 delta = glm::vec4(allBulletDirs[bulletIdx] * deltaPosMagnitude, 0.0f);
                        m_offsets[bulletIdx] += delta;
                    }
                }
            }));
        }
    }
    for (auto& future : futures) {
        future.get();
    }
    int firstLivingBullet = 0;
    if (firstLiveBulletGroup != 0) {
        firstLivingBullet = bulletGroups[firstLiveBulletGroup - 1].startIndex + bulletGroups[firstLiveBulletGroup - 1].groupSize;
        bulletGroups.erase(bulletGroups.begin(), bulletGroups.begin() + firstLiveBulletGroup);
    }
    if (firstLivingBullet != 0) {
        m_offsets.erase(m_offsets.begin(), m_offsets.begin() + firstLivingBullet);
        m_rots.erase(m_rots.begin(), m_rots.begin() + firstLivingBullet);
        allBulletDirs.erase(allBulletDirs.begin(), allBulletDirs.begin() + firstLivingBullet);
        for (BulletGroup& g : bulletGroups) {
            g.startIndex -= firstLivingBullet;
        }
    }
}