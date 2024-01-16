#pragma once

#include <engine/Clock.h>
#include <engine/Camera.h>
#include "Entity.h"
#include "ObjSequence.h"


class AntNew : public Entity {

public:

	AntNew(const ObjSequence& objSequence, std::shared_ptr<aw::Mesh> mesh, aw::Material material, Entity* target);

	void start();
	void update(const float dt);
	void draw(const Camera& camera) override;
	void fixedUpdate(float fdt) override;

	void damage(unsigned int amount);
	unsigned getHp();
	void die();
	bool timeToDestroy();

	Clock timeSinceDealtDamage;

private:

	void animate(float deltaTime);
	float speed = 8.0f;
	float detectionRaduis = 25.0f;
	float animTime = 0.0f;
	float walkcycleLength = 1.0f;
	Entity* target;

	int index = 0;
	int baseIndex = 0;
	const ObjSequence& objSequence;

	unsigned int hp = 5;
	Clock timeSinceDamage, timeSinceLastAlphaDecrease;
	aw::Material originalMaterial;
	bool isHurting = false;
	bool isAlive = true;
};
