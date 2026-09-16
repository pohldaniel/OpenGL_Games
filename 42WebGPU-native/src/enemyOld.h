#pragma once
#include "glm/glm.hpp"

struct EnemyOld {
    glm::vec3 position;
    glm::vec3 dir;

    EnemyOld(glm::vec3 _position, glm::vec3 _dir)
        : position(std::move(_position)), dir(std::move(_dir)) {
    }
};