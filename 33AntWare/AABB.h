#pragma once
#include <glm/glm.hpp>

namespace aw{

  class AABB {

  public:

    float up, down, right, left, forward, backward;

 

    glm::vec3 bounds[8];
    bool isColliding(const AABB &other);
    bool isColliding(glm::vec3 point);
    glm::vec3 getCenter();
    void minmize(float factor);
    void maximize(float factor);

  };
}