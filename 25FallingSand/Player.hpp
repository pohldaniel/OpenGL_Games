#pragma once

#include "Entity.hpp"
#include "Item.hpp"

class World;

class Player : public Entity {
public:
    Item* heldItem = nullptr;
    float holdAngle = 0;
    long long startThrow = 0;
    bool holdHammer = false;
    bool holdVacuum = false;
    int hammerX = 0;
    int hammerY = 0;

    void render(GPU_Target* target, int ofsX, int ofsY) override;
    void renderLQ(GPU_Target* target, int ofsX, int ofsY) override;
    void setItemInHand(Item* item, World* world);

    ~Player();
};
