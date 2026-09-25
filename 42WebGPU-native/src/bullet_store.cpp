#define _USE_MATH_DEFINES
#include <math.h>
#include <chrono>
#include <algorithm>
#include <iostream>
#include <thread>


#include <Entities/Enemy.h>

#include "aabb.h"
#include "bullet_store.h"

namespace {

const float pi = (float)M_PI;
const float rotPerBullet = 3.0f * pi / 180.0f;

const float bulletScale = 0.3f;
const float bulletLifetime = 1.0f; // seconds
const Vector3f scaleVec(bulletScale, bulletScale, bulletScale);
const float bulletSpeed = 15.0f; // Game units per second
const Vector3f bulletNormal(0.0f, 1.0f, 0.0f);
const Vector3f canonicalDir(0.0f, 0.0f, 1.0f);

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
    const Vector3f& a0,
    const Vector3f& a1,
    const Vector3f& b0,
    const Vector3f& b1) {
    const float EPS = 0.001f;

    const Vector3f A = a1 - a0;
    const Vector3f B = b1 - b0;
    const float magA = A.length();
    const float magB = B.length();

    const Vector3f _A = Vector3f::Normalize(A);
    const Vector3f _B = Vector3f::Normalize(B);

    const Vector3f cross = Vector3f::Cross(_A, _B);
    const float cl = cross.length();
    const float denom = cl * cl;


    // If lines are parallel (denom=0) test if lines overlap.
    // If they don't overlap then there is a closest point solution.
    // If they do overlap, there are infinite closest positions, but there is a closest distance
    if (denom < EPS) {
        const float d0 = Vector3f::Dot(_A, (b0 - a0));
        const float d1 = Vector3f::Dot(_A, (b1 - a0));

        // Is segment B before A?
        if (d0 <= 0.0f && 0.0f >= d1) {
            if (abs(d0) < abs(d1)) {
                return (a0 - b0).length();
            }
            return (a0 - b1).length();
        }
        else if (d0 >= magA && magA <= d1) {
            if (abs(d0) < abs(d1)) {
                return (a1 - b0).length();
            }
            return (a1 - b1).length();
        }


        // Segments overlap, return distance between parallel segments
        return (((d0 * _A) + a0) - b0).length();
    }

    

    // Lines criss-cross: Calculate the projected closest points
    const Vector3f t = (b0 - a0);
    const float detA = Vector3f::Dot(t, Vector3f::Cross(_B, cross));
    const float detB = Vector3f::Dot(t, Vector3f::Cross(_A, cross));

    const float t0 = detA / denom;
    const float t1 = detB / denom;

    Vector3f pA = a0 + (_A * t0); // Projected closest point on segment A
    Vector3f pB = b0 + (_B * t1); // Projected closest point on segment B

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
        float dot = Vector3f::Dot(_B, (pA - b0));
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
        float dot = Vector3f::Dot(_A, (pB - a0));
        if (dot < 0.0f) {
            dot = 0;
        }
        else if (dot > magA) {
            dot = magA;
        }
        pA = a0 + (_A * dot);
    }

    return (pA - pB).length();
}

bool bulletCollidesWithEnemy(const Vector3f& bPos, const Vector3f& bDir, const Enemy* e) {
    const Vector3f& ePos = e->getPosition();
    const Vector3f& eDir = e->getDirection();


    if ((bPos - ePos).lengthSq() > bulletEnemyMaxCollisionDist2) {
        return false;
    }
    const float closestDist =
        distanceBetweenLineSegments(
            bPos - bDir * (0.3f / 2),
            bPos + bDir * (0.3f / 2),
            ePos - eDir * (0.4f / 2),
            ePos + eDir * (0.4f / 2));
    return closestDist <= (0.03f + 0.08f);
}

}  // namespace


void BulletStore::createBullets(const Vector3f& position, const Quaternion& midOri, const int spreadAmount) {
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
        const Quaternion yQuat  = midOri * Quaternion::Rotate(0.0f, (rotPerBullet * (i - spreadAmount / 2)) * _180_ON_PI - 90.0f , 0.0f);
        for (int j = 0; j < spreadAmount; ++j) {
          const Quaternion rotQuat = yQuat * Quaternion::Rotate(0.0f, 0.0f, rotPerBullet * (j - spreadAmount / 2) * _180_ON_PI);          
          const Vector3f dir = Quaternion::Rotate(rotQuat, Vector3f::RIGHT);
          const int pos = i * spreadAmount + j + startIndex;
          m_offsets[pos] = Vector4f(position - dir * 0.2f, 0.0f) ;
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
                        Vector4f delta = Vector4f(allBulletDirs[bulletIdx] * deltaPosMagnitude, 0.0f);
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