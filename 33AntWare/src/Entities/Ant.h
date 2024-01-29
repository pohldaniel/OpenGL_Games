#pragma once

#include <engine/Clock.h>
#include <Entities/Entity.h>

#include "ObjSequence.h"

class Ant : public Entity {

public:
    
	Ant(const ObjSequence& objSequence, AssimpModel* model, Entity *target);

	void start();
	void update(const float dt) override;
	void draw(const Camera& camera) override;
	void fixedUpdate(float fdt) override;

	void damage(unsigned int amount);
	unsigned getHp();
	void die();
	bool timeToDestroy();
	Clock timeSinceDealtDamage;

private:

	void animate(float deltaTime);
	const Material& getMaterial() const override;

	float speed = 8.0f;
	float detectionRaduis = 25.0f;
	float animTime = 0.0f;
	float walkcycleLength = 1.0f;
	Entity *target;

	int index = 0;
	int baseIndex = 0;
	const ObjSequence& objSequence;

	unsigned int hp = 5;
	Clock timeSinceDamage, timeSinceLastAlphaDecrease;
	bool isHurting = false;
	bool isAlive = true;

	Material m_material;
};
