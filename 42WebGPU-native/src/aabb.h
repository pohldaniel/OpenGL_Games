#pragma once

#include <float.h>
#include <algorithm>
#include <engine/Vector.h>

struct AABB {
    float xMin = FLT_MAX;
    float xMax = FLT_MIN;
    float yMin = FLT_MAX;
    float yMax = FLT_MIN;
    float zMin = FLT_MAX;
    float zMax = FLT_MIN;

    void expandToInclude(const Vector4f& v) {
        xMin = std::min(xMin, v[0]);
        xMax = std::max(xMax, v[0]);
        yMin = std::min(yMin, v[1]);
        yMax = std::max(yMax, v[1]);
        zMin = std::min(zMin, v[2]);
        zMax = std::max(zMax, v[2]);
        isInitialised = true;
    }

    void expandBy(const float f) {
        if (!isInitialised) return;
        xMin -= f;
        xMax += f;
        yMin -= f;
        yMax += f;
        zMin -= f;
        zMax += f;
    }

    bool containsPoint(float x, float y, float z) const {
        return x >= xMin
            && x <= xMax
            && y >= yMin
            && y <= yMax
            && z >= zMin
            && z <= zMax;
    }

private:
    bool isInitialised = false;
};

bool AABBsIntersect(const AABB& a, const AABB& b) {
    return a.containsPoint(b.xMin, b.yMin, b.zMin)
        || a.containsPoint(b.xMin, b.yMin, b.zMax)
        || a.containsPoint(b.xMin, b.yMax, b.zMin)
        || a.containsPoint(b.xMin, b.yMax, b.zMax)
        || a.containsPoint(b.xMax, b.yMin, b.zMin)
        || a.containsPoint(b.xMax, b.yMin, b.zMax)
        || a.containsPoint(b.xMax, b.yMax, b.zMin)
        || a.containsPoint(b.xMax, b.yMax, b.zMax);
}