#include "bullet_store.h"

#include <chrono>
#include <algorithm>
#include <iostream>
#include <thread>
#include <Entities/Enemy.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/vector_angle.hpp>

#include "aabb.h"

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

const float bulletEnemyMaxCollisionDist = 0.3f / 2 + 0.03f + 0.4f / 2 + 0.08f;
const float bulletEnemyMaxCollisionDist2 = bulletEnemyMaxCollisionDist * bulletEnemyMaxCollisionDist;

float distanceBetweenLineSegments(
    const glm::vec3& a0,
    const glm::vec3& a1,
    const glm::vec3& b0,
    const glm::vec3& b1) {
    const float EPS = 0.001f;

    const glm::vec3 A = a1 - a0;
    const glm::vec3 B = b1 - b0;
    const float magA = glm::length(A);
    const float magB = glm::length(B);

    const glm::vec3 _A = A / magA;
    const glm::vec3 _B = B / magB;

    const glm::vec3 cross = glm::cross(_A, _B);
    const float cl = glm::length(cross);
    const float denom = cl * cl;


    // If lines are parallel (denom=0) test if lines overlap.
    // If they don't overlap then there is a closest point solution.
    // If they do overlap, there are infinite closest positions, but there is a closest distance
    if (denom < EPS) {
        const float d0 = glm::dot(_A, (b0 - a0));
        const float d1 = glm::dot(_A, (b1 - a0));

        // Is segment B before A?
        if (d0 <= 0.0f && 0.0f >= d1) {
            if (abs(d0) < abs(d1)) {
                return glm::length(a0 - b0);
            }
            return glm::length(a0 - b1);
        }
        else if (d0 >= magA && magA <= d1) {
            if (abs(d0) < abs(d1)) {
                return glm::length(a1 - b0);
            }
            return glm::length(a1 - b1);
        }


        // Segments overlap, return distance between parallel segments
        return glm::length(((d0 * _A) + a0) - b0);
    }



    // Lines criss-cross: Calculate the projected closest points
    const glm::vec3 t = (b0 - a0);
    const float detA = glm::determinant(glm::mat3(t, _B, cross));
    const float detB = glm::determinant(glm::mat3(t, _A, cross));

    const float t0 = detA / denom;
    const float t1 = detB / denom;

    glm::vec3 pA = a0 + (_A * t0); // Projected closest point on segment A
    glm::vec3 pB = b0 + (_B * t1); // Projected closest point on segment B

    // Clamp projections
    if (t0 < 0.0f) {
        pA = a0;
    }
    else if (t0 > magA) {
        pA = a1;
    }

    if (t1 < 0) {
        pB = b0;
    }
    else if (t1 > magB) {
        pB = b1;
    }

    // Clamp projection A
    if (t0 < 0.0f || t0 > magA) {
        float dot = glm::dot(_B, (pA - b0));
        if (dot < 0.0f) {
            dot = 0;
        }
        else if (dot > magB) {
            dot = magB;
        }
        pB = b0 + (_B * dot);
    }

    // Clamp projection B
    if (t1 < 0.0f || t1 > magB) {
        float dot = glm::dot(_A, (pB - a0));
        if (dot < 0.0f) {
            dot = 0;
        }
        else if (dot > magA) {
            dot = magA;
        }
        pA = a0 + (_A * dot);
    }

    return glm::length(pA - pB);
}

bool bulletCollidesWithEnemy(const glm::vec3& bPos, const glm::vec3& bDir, const Enemy* e) {
    const Vector3f& ePos = e->getPosition();
    const Vector3f& eDir = e->getDirection();
    glm::vec3 pos = glm::vec3(ePos[0], ePos[1], ePos[2]);
    glm::vec3 dir = glm::vec3(eDir[0], eDir[1], eDir[2]);

    if (glm::distance2(bPos, pos) > bulletEnemyMaxCollisionDist2) {
        return false;
    }
    const float closestDist =
        distanceBetweenLineSegments(
            bPos - bDir * (0.3f / 2),
            bPos + bDir * (0.3f / 2),
            pos - dir * (0.4f / 2),
            pos + dir * (0.4f / 2));
    return closestDist <= (0.03f + 0.08f);
}

}  // namespace


void BulletStore::createBullets(const glm::vec3& position, const glm::quat& midOri, const int spreadAmount) {
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
          m_offsets[pos] = glm::vec4(position - dir * 0.2f, 0.0f) ;
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

void BulletStore::updateBullets(float deltaTimeSeconds, std::vector<Enemy*>& enemies) {

    // Bullet groups are divided into subgroups, which are excluded en masse from
    // enemy collision detection.
    const bool useAABB = enemies.size() > 0;
    const int numSubGroups = useAABB ? 9 : 1;

    const float deltaPosMagnitude = deltaTimeSeconds * bulletSpeed;
    int firstLiveBulletGroup = 0;

    // True iff enemies[i] is dead from bullet collision.
    std::vector<bool> enemyDeathMarker(enemies.size());

    std::vector<std::future<void>> futures;
    for (BulletGroup& g : bulletGroups) {
        g.TTL -= deltaTimeSeconds;
        if (g.TTL <= 0.0f) {
            firstLiveBulletGroup++;
        }
        else {
            futures.emplace_back(threadPool->enqueue([this, numSubGroups, useAABB, deltaPosMagnitude, &g, &enemyDeathMarker, &enemies]() {
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

                    AABB subgroupBoundingBox;
                    if (useAABB) {
                        for (int bulletIdx = bulletsStart; bulletIdx < bulletsEnd; ++bulletIdx) {
                            subgroupBoundingBox.expandToInclude(m_offsets[bulletIdx]);
                        }
                        subgroupBoundingBox.expandBy(bulletEnemyMaxCollisionDist);
                    }

                    for (int i = 0; i < enemies.size(); ++i) {
                        const Enemy* e = enemies.at(i);
                        const Vector3f& pos = e->getPosition();
                        if (useAABB && !subgroupBoundingBox.containsPoint(pos[0], pos[1], pos[2])) {
                            continue;
                        }
                        for (int bulletIdx = bulletsStart; bulletIdx < bulletsEnd; ++bulletIdx) {
                            if (bulletCollidesWithEnemy(m_offsets[bulletIdx], allBulletDirs[bulletIdx], e)) {
                                // TODO kill bullet too? ... angry bots ECS version doesn't...
                                enemyDeathMarker[i] = true;
                                break;
                            }
                        }
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

    for (int i = (enemies.size() - 1); i >= 0; --i) {
        if (enemyDeathMarker[i]) {
            (enemies)[i]->setIsDeath(true);
            //enemyDeathSprites->emplace_back((*enemies)[i].position);
            //enemies->erase(enemies->begin() + i);
        }
    }
}