#pragma once

#include <engine/Clock.h>
#include "ObjSequence.h"
#include "GameObject.h"
#include "Mesh.h"
#include "MaterialAW.h"

class Ant : public aw::GameObject {

public:
    
	Ant(const ObjSequence& objSequence, std::shared_ptr<aw::Mesh> mesh, aw::Material material, GameObject *parent, GameObject *target);

	void start() override;
	void update(const float dt);
	void draw() override;
	void fixedUpdate(float fdt) override;

	void damage(unsigned int amount);
	unsigned getHp();
	void die();
	bool timeToDestroy();

private:

	void animate(float deltaTime);
	float speed = 8.0f;
	float detectionRaduis = 25.0f;
	float animTime = 0.0f;
	float walkcycleLength = 1.0f;
	GameObject *target;

	int index = 0;
	int baseIndex = 0;
	const ObjSequence& objSequence;

	unsigned int hp = 5;
	Clock timeSinceDamage, timeSinceLastAlphaDecrease;
	aw::Material originalMaterial;
	bool isHurting = false;
	bool isAlive = true;
};
