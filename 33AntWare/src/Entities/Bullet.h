#pragma once
#include <engine/Clock.h>
#include "Entity.h"

class Bullet : public Entity {

public:

	Bullet(Vector3f projectVec);
	Bullet(Bullet const& rhs);
	Bullet& operator=(const Bullet& rhs);

	void start();
	void update(const float dt);
	bool timeOut();

	static void Init(std::shared_ptr<aw::Mesh> mesh, aw::Material material);



	float lifeSpan = 5.0f;
	float speed = 40.0f;
	Clock timer;

	static std::shared_ptr<aw::Mesh> Mesh;
	static aw::Material Material;
};