#pragma once

#include <engine/interfaces/ICounter.h>
#include <engine/Clock.h>

#include <Entities/Entity.h>

#include "ObjSequence.h"
#include "Player.h"


class Ant : public Entity, private ICounter<Ant>{

public:
    
	Ant(const ObjSequence& objSequence, AssimpModel* model, Player* target);
	~Ant() = default;

	void update(const float dt) override;
	void draw() override;


	void damage(unsigned int amount);
	unsigned getHp();
	void die();
	bool timeToDestroy();
	Clock timeSinceDealtDamage;
	using ICounter<Ant>::GetCount;

private:

	void animate(float deltaTime);
	const Material& getMaterial() const override;

	float speed = 8.0f;
	float detectionRaduis = 25.0f;
	float animTime = 0.0f;
	float walkcycleLength = 1.0f;
	Player* target;

	int index = 0;
	int baseIndex = 0;
	const ObjSequence& objSequence;

	unsigned int hp = 5;
	Clock timeSinceDamage, timeSinceLastAlphaDecrease;
	bool isHurting = false;
	bool isAlive = true;

	Material m_material;
};
