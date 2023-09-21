#pragma once

#include <unordered_map>
#include <box2d/b2_distance_joint.h>

#define ALIGN_LEFT 0
#define ALIGN_CENTER 1
#define ALIGN_RIGHT 2

class Drawing {
public:

    static b2Vec2 rotate_point(float cx, float cy, float angle, b2Vec2 p);
    static uint32 darkenColor(uint32 col, float brightness);

};
