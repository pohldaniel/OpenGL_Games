#pragma once

#include <memory>
#include "Mesh.h"
#include "AABB.h"

namespace aw{

  class Ant {
  protected:

    unsigned hp = 5;
   
    bool isHurting = false;
    bool isAlive = true;
   
    virtual void die();

  public:
  
    Ant(std::shared_ptr<Mesh> mesh);
   
	virtual void start();
	virtual void update();
	virtual void fixedUpdate(float deltaTime);
	virtual void draw();

    void damage(unsigned amount);
    unsigned getHp();
    bool timeToDestroy();

	AABB aabb;
	std::shared_ptr<Mesh> meshPtr;
  };
}