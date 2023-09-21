#include "Entity.hpp"
#include "RigidBody.hpp"

void Entity::renderLQ(GPU_Target* target, int ofsX, int ofsY) {
    GPU_Rectangle(target, x + ofsX, y + ofsY, x + ofsX + hw, y + ofsY + hh, {0xff, 0, 0, 0xff});
}


void Entity::render(GPU_Target* target, int ofsX, int ofsY) {
}

Entity::~Entity() {
    //delete rb;
}
